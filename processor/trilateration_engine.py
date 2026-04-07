import numpy as np
from scipy.optimize import least_squares

def solve_position(anchor_coords, distances):
    """
    Solves for the aircraft position using Least Squares trilateration.
    
    :param anchor_coords: List of (x, y, z) tuples for ground stations.
    :param distances: List of distances (m) from each anchor to the aircraft.
    :return: Estimated (x, y, z) position of the aircraft.
    """
    anchor_coords = np.array(anchor_coords)
    distances = np.array(distances)
    
    def residuals(p, anchors, dists):
        """Calculates difference between p and each anchor distance."""
        return np.linalg.norm(anchors - p, axis=1) - dists

    # Initial guess: Average of anchor positions + small Z offset to avoid stalling on coplanar anchors
    initial_guess = np.mean(anchor_coords, axis=0)
    initial_guess[2] = 50.0 # Assume aircraft is airborne
    
    res = least_squares(residuals, initial_guess, args=(anchor_coords, distances))
    
    return res.x

if __name__ == "__main__":
    # Example usage:
    anchors = [
        (0, 0, 0),
        (100, 0, 0),
        (50, 86.6, 0)
    ]
    # Assume aircraft is at (50, 28, 50)
    true_pos = np.array([50, 28, 50])
    measured_distances = np.linalg.norm(np.array(anchors) - true_pos, axis=1)
    
    estimated_pos = solve_position(anchors, measured_distances)
    print(f"True Position:      {true_pos}")
    print(f"Estimated Position: {estimated_pos}")
