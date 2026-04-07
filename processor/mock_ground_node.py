import socket
import time
import random
import json
import math
import numpy as np

UDP_IP = "127.0.0.1"
UDP_PORT = 12345
ANCHORS_FILE = "anchors.json"

def gps_to_local_enu(lat, lon, alt, ref_lat, ref_lon, ref_alt):
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

def send_mock_data():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # Load anchors and convert to local ENU
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
    
    # Initial target position in Local ENU
    target = np.array([20.0, 30.0, 50.0])
    velocity = np.array([2.0, 1.0, 0.5]) # m/s
    
    print(f"Simulating aircraft starting at: {target} with velocity {velocity}")
    
    last_time = time.time()
    while True:
        now = time.time()
        dt = now - last_time
        target = target + velocity * dt
        last_time = now
        
        for s_id, pos in anchors_cartesian.items():
            true_dist = np.linalg.norm(np.array(pos) - target)
            dist = true_dist + random.uniform(-0.02, 0.02)
            
            msg = f"S:{s_id},T:T01,D:{dist:.2f}"
            sock.sendto(msg.encode('utf-8'), (UDP_IP, UDP_PORT))
            time.sleep(0.1)
        
        print(f"Aircraft at: {target[0]:.1f}, {target[1]:.1f}, {target[2]:.1f}")

if __name__ == "__main__":
    send_mock_data()
