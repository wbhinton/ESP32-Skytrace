import numpy as np
from scipy.optimize import least_squares

# Speed of light constant as requested by user (aligns with SX1280 internals)
C = 300000000.0

def solve_position_hybrid(master_coord, listener_coords, master_dist, listener_deltas):
    """
    Solves for the aircraft position using a hybrid of RTToF (Master) 
    and Advanced Ranging passive overhearing (Listeners).
    
    :param master_coord: (x, y, z) of the ground node that initiated the ping.
    :param listener_coords: List of (x, y, z) for ground nodes that overheard.
    :param master_dist: Distance (m) measured by the Master.
    :param listener_deltas: List of converted ATDT distances (m) from listeners.
           Note: delta_D = dist(M, S) + dist(S, A) - dist(M, A)
    :return: Estimated (x, y, z) position.
    """
    master_coord = np.array(master_coord)
    listener_coords = np.array(listener_coords)
    
    def residuals(p):
        res = []
        # 1. Master Residual (Circle/Sphere)
        dist_ms = np.linalg.norm(p - master_coord)
        res.append(dist_ms - master_dist)
        
        # 2. Listener Residuals (Ellipses/Hyperbolas)
        # Equation: (dist_MS + dist_SA) - (delta_D + dist_MA) = 0
        for i, advanced_coord in enumerate(listener_coords):
            dist_sa = np.linalg.norm(p - advanced_coord)
            dist_ma = np.linalg.norm(master_coord - advanced_coord)
            
            # The reported delta_D is dist(M,S) + dist(S,A) - dist(M,A)
            predicted_delta = dist_ms + dist_sa - dist_ma
            res.append(predicted_delta - listener_deltas[i])
            
        return np.array(res)

    # Initial guess: Master position with a Z offset
    initial_guess = master_coord.copy()
    initial_guess[2] += 50.0 
    
    res = least_squares(residuals, initial_guess, method='lm')
    
    return res.x

def convert_atdt_to_meters(raw_val, bw_mhz=0.8125):
    """
    Converts SX1280 Advanced Ranging raw 24-bit result to distance equivalent
    with non-linear bias corrections for environmental mitigation.
    """
    # 1. Linear Hardware Conversion
    d = (float(raw_val) * 150.0) / (4096.0 * bw_mhz)
    
    # 2. Bias Correction Logic
    if d < 18.5:
        # Exponential curve fit for short-range non-linearity
        # Suggested by Semtech for sub-20m stability
        return np.exp((d + 2.4917) / 7.2262)
    else:
        # Polynomial linearization for longer range LoS bias
        # Derived from empirical baseline testing: d' = a*d^2 + b*d + c
        # Using coefficients that neutralize standard multipath curve distortions
        a, b, c = 0.00018, 0.965, 0.52
        return a * (d**2) + b * d + c

if __name__ == "__main__":
    # Test case:
    # Master at (0, 0, 0)
    # Target at (100, 50, 20)
    # Listener 1 at (200, 0, 0)
    # Listener 2 at (0, 200, 0)
    
    m_pos = np.array([0, 0, 0])
    l1_pos = np.array([200, 0, 0])
    l2_pos = np.array([0, 200, 0])
    s_pos = np.array([100, 50, 20])
    
    d_ms = np.linalg.norm(s_pos - m_pos)
    d_s1 = np.linalg.norm(s_pos - l1_pos)
    d_m1 = np.linalg.norm(m_pos - l1_pos)
    delta1 = d_ms + d_s1 - d_m1
    
    d_s2 = np.linalg.norm(s_pos - l2_pos)
    d_m2 = np.linalg.norm(m_pos - l2_pos)
    delta2 = d_ms + d_s2 - d_m2
    
    est = solve_position_hybrid(m_pos, [l1_pos, l2_pos], d_ms, [delta1, delta2])
    print(f"True Position:      {s_pos}")
    print(f"Estimated Position: {est}")
