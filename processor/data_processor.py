import socket
import json
import time
import math
import os
import csv
import numpy as np
from trilateration_engine import solve_position

# Configuration
UDP_IP = "0.0.0.0"
UDP_PORT = 12345
ANCHORS_FILE = "anchors.json"
LOG_DIR = "logs"

# State
latest_distances = {} # {target_id: {station_id: distance}}
anchors_gps = {}
anchors_cartesian = {}
target_telemetry = {} # {target_id: {'pos': np.array, 'vel': np.array, 'time': float, 'logger': obj}}

class TelemetryLogger:
    def __init__(self, target_id):
        os.makedirs(LOG_DIR, exist_ok=True)
        self.filename = os.path.join(LOG_DIR, f"flight_{target_id}.csv")
        file_exists = os.path.isfile(self.filename)
        self.file = open(self.filename, 'a', newline='')
        self.writer = csv.writer(self.file)
        if not file_exists:
            self.writer.writerow(['timestamp', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'ax', 'ay', 'az'])

    def log(self, ts, pos, vel, acc):
        self.writer.writerow([ts, *pos, *vel, *acc])
        self.file.flush()

def gps_to_local_enu(lat, lon, alt, ref_lat, ref_lon, ref_alt):
    """Converts GPS to Local Cartesian (East, North, Up) in meters."""
    R = 6378137.0 
    lat_rad = math.radians(lat)
    lon_rad = math.radians(lon)
    ref_lat_rad = math.radians(ref_lat)
    ref_lon_rad = math.radians(ref_lon)

    d_lat = lat_rad - ref_lat_rad
    d_lon = lon_rad - ref_lon_rad

    y = d_lat * R
    x = d_lon * R * math.cos(ref_lat_rad)
    z = alt - ref_alt
    return [x, y, z]

def load_anchors():
    global anchors_gps, anchors_cartesian
    try:
        with open(ANCHORS_FILE, 'r') as f:
            anchors_gps = json.load(f)
        
        ref_id = next((k for k, v in anchors_gps.items() if v.get("is_reference")), "G01")
        ref = anchors_gps[ref_id]
        
        anchors_cartesian = {}
        for s_id, pos in anchors_gps.items():
            anchors_cartesian[s_id] = gps_to_local_enu(
                pos['lat'], pos['lon'], pos['alt'],
                ref['lat'], ref['lon'], ref['alt']
            )
        return ref_id
    except Exception as e:
        print(f"Error loading anchors: {e}")
        return None

def update_telemetry(t_id, current_pos, current_time):
    state = target_telemetry.get(t_id)
    vel = np.zeros(3)
    acc = np.zeros(3)
    
    if state:
        dt = current_time - state['time']
        if dt > 0:
            vel = (current_pos - state['pos']) / dt
            if 'last_vel' in state:
                acc = (vel - state['last_vel']) / dt
        logger = state['logger']
    else:
        logger = TelemetryLogger(t_id)
    
    target_telemetry[t_id] = {
        'pos': current_pos,
        'last_vel': vel,
        'time': current_time,
        'logger': logger
    }
    
    logger.log(current_time, current_pos, vel, acc)
    return vel, acc

def parse_packet(data_str):
    """S:STATION_ID,T:TARGET_ID,D:DISTANCE"""
    try:
        parts = data_str.split(',')
        s_id = parts[0].split(':')[1]
        t_id = parts[1].split(':')[1]
        dist = float(parts[2].split(':')[1])
        return s_id, t_id, dist
    except Exception:
        return None, None, None

def run_processor():
    ref_id = load_anchors()
    if not ref_id: return
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))
    print(f"Listening on {UDP_IP}:{UDP_PORT}...")
    
    while True:
        data, addr = sock.recvfrom(1024)
        s_id, t_id, dist = parse_packet(data.decode('utf-8'))
        
        if s_id and s_id in anchors_cartesian:
            if t_id not in latest_distances:
                latest_distances[t_id] = {}
            
            latest_distances[t_id][s_id] = dist
            
            if len(latest_distances[t_id]) >= 3:
                coords = []
                dists = []
                for station_id, d in latest_distances[t_id].items():
                    coords.append(anchors_cartesian[station_id])
                    dists.append(d)
                
                pos = solve_position(coords, dists)
                now = time.time()
                vel, acc = update_telemetry(t_id, pos, now)
                
                v_mag = np.linalg.norm(vel)
                print(f">>> Target {t_id} | Pos: {pos[0]:.1f},{pos[1]:.1f},{pos[2]:.1f} | Vel: {v_mag:.1f}m/s")

if __name__ == "__main__":
    run_processor()
