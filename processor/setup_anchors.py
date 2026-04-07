import json

def setup():
    print("--- Skytrace Anchor Setup ---")
    anchors = {}
    
    for i in range(1, 4):
        print(f"\nGround Station {i}:")
        lat = float(input(f"  Latitude (dd): "))
        lon = float(input(f"  Longitude (dd): "))
        alt = float(input(f"  Altitude (m): "))
        
        anchors[f"G0{i}"] = {
            "lat": lat,
            "lon": lon,
            "alt": alt,
            "is_reference": (i == 1)
        }
        
    with open("anchors.json", "w") as f:
        json.dump(anchors, f, indent=4)
        
    print("\nSaved anchors to anchors.json")

if __name__ == "__main__":
    setup()
