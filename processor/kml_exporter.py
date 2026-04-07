import csv
import json
import math
import os

def local_enu_to_gps(x, y, z, ref_lat, ref_lon, ref_alt):
    """Converts local Cartesian (ENU) meters back to GPS (Lat, Lon, Alt)."""
    R = 6378137.0
    ref_lat_rad = math.radians(ref_lat)
    
    d_lat = y / R
    d_lon = x / (R * math.cos(ref_lat_rad))
    
    lat = math.degrees(ref_lat_rad + d_lat)
    lon = math.degrees(math.radians(ref_lon) + d_lon)
    alt = z + ref_alt
    return lat, lon, alt

def export_to_kml(csv_path, anchors_file, output_path):
    # Load reference station from anchors.json
    with open(anchors_file, 'r') as f:
        anchors = json.load(f)
    ref = next((v for k, v in anchors.items() if v.get("is_reference")), list(anchors.values())[0])
    
    coordinates = []
    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            x, y, z = float(row['x']), float(row['y']), float(row['z'])
            lat, lon, alt = local_enu_to_gps(x, y, z, ref['lat'], ref['lon'], ref['alt'])
            coordinates.append(f"{lon},{lat},{alt}")
            
    kml_content = f"""<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2">
  <Document>
    <name>Skytrace Flight Path</name>
    <Style id="yellowLineGreenPoly">
      <LineStyle>
        <color>7f00ffff</color>
        <width>4</width>
      </LineStyle>
    </Style>
    <Placemark>
      <name>Path</name>
      <styleUrl>#yellowLineGreenPoly</styleUrl>
      <LineString>
        <extrude>1</extrude>
        <tessellate>1</tessellate>
        <altitudeMode>absolute</altitudeMode>
        <coordinates>
          {" ".join(coordinates)}
        </coordinates>
      </LineString>
    </Placemark>
  </Document>
</kml>
"""
    with open(output_path, 'w') as f:
        f.write(kml_content)
    print(f"Exported KML to: {output_path}")

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python3 kml_exporter.py <csv_log_file>")
    else:
        csv_file = sys.argv[1]
        out_file = csv_file.replace(".csv", ".kml")
        export_to_kml(csv_file, "anchors.json", out_file)
        
