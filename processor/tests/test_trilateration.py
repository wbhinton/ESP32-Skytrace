import numpy as np
import pytest
from trilateration_engine import solve_position

def test_trilateration_basic():
    """
    Test case from user requirements:
    Anchors: (0,0,0), (10,0,0), (0,10,0)
    Distances: (5, 5, 7.07)
    Result should be: (5, 5, 0)
    """
    anchors = [
        (0, 0, 0),
        (10, 0, 0),
        (0, 10, 0)
    ]
    distances = [5, 5, 7.07]
    
    expected_pos = np.array([5, 5, 0])
    estimated_pos = solve_position(anchors, distances)
    
    # Check if estimated position is close to expected position within a margin of 0.1
    np.testing.assert_allclose(estimated_pos, expected_pos, atol=0.1)
