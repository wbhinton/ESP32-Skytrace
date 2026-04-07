import pytest
import numpy as np
from data_processor import process_update, parse_packet

def test_single_target_trilateration():
    # Setup anchors
    anchors = {
        "G01": [0, 0, 0],
        "G02": [10, 0, 0],
        "G03": [0, 10, 0]
    }
    state = {}
    
    # Target at (5, 5, 2)
    t_id = "T01"
    target_pos = np.array([5, 5, 2])
    
    # Simulate updates from 3 stations
    for s_id, a_pos in anchors.items():
        dist = np.linalg.norm(np.array(a_pos) - target_pos)
        result = process_update(s_id, t_id, dist, anchors, state)
        
    # Final result should be close to (5, 5, 2)
    # Note: Z ambiguity with coplanar anchors might lead to Z=0 or Z=2
    assert result is not None
    assert np.allclose(result[:2], [5, 5], atol=0.1)

def test_multi_target_trilateration():
    anchors = {
        "G01": [0, 0, 0],
        "G02": [10, 0, 0],
        "G03": [0, 10, 0]
    }
    state = {}
    
    targets = {
        "T01": [2, 2, 0],
        "T02": [8, 8, 0],
        "T03": [5, 5, 5]
    }
    
    # Simulate interleaved updates from all stations to all targets
    for t_id, t_pos in targets.items():
        t_pos_arr = np.array(t_pos)
        for s_id, a_pos in anchors.items():
            dist = np.linalg.norm(np.array(a_pos) - t_pos_arr)
            # Simulate a packet string
            packet = f"S:{s_id},T:{t_id},D:{dist:.4f}"
            ps_id, pt_id, pdist = parse_packet(packet)
            
            result = process_update(ps_id, pt_id, pdist, anchors, state)
            
            if s_id == "G03": # Last station for this target
                assert result is not None
                print(f"Target {t_id} solved as {result}")
                assert np.allclose(result[:2], t_pos[:2], atol=0.1)

def test_packet_parsing():
    packet = "S:G01,T:TRANS_99,D:123.45"
    s_id, t_id, dist = parse_packet(packet)
    assert s_id == "G01"
    assert t_id == "TRANS_99"
    assert dist == 123.45

    invalid = "S:G01,DIST:12.34"
    s_id, t_id, dist = parse_packet(invalid)
    assert s_id is None
