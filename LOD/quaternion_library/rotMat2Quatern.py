import numpy as np

def rotMat2quatern(R):
    """
    회전 행렬을 쿼터니언으로 변환합니다.
    
    Args:
    R (numpy.ndarray): (3, 3, n) 형태의 회전 행렬

    Returns:
    numpy.ndarray: (n, 4) 형태의 쿼터니언
    """
    numR = R.shape[2]
    q = np.zeros((numR, 4))
    K = np.zeros((4, 4))
    
    for i in range(numR):
        K[0, 0] = (1/3) * (R[0, 0, i] - R[1, 1, i] - R[2, 2, i])
        K[0, 1] = (1/3) * (R[1, 0, i] + R[0, 1, i])
        K[0, 2] = (1/3) * (R[2, 0, i] + R[0, 2, i])
        K[0, 3] = (1/3) * (R[1, 2, i] - R[2, 1, i])
        K[1, 0] = (1/3) * (R[1, 0, i] + R[0, 1, i])
        K[1, 1] = (1/3) * (R[1, 1, i] - R[0, 0, i] - R[2, 2, i])
        K[1, 2] = (1/3) * (R[2, 1, i] + R[1, 2, i])
        K[1, 3] = (1/3) * (R[2, 0, i] - R[0, 2, i])
        K[2, 0] = (1/3) * (R[2, 0, i] + R[0, 2, i])
        K[2, 1] = (1/3) * (R[2, 1, i] + R[1, 2, i])
        K[2, 2] = (1/3) * (R[2, 2, i] - R[0, 0, i] - R[1, 1, i])
        K[2, 3] = (1/3) * (R[0, 1, i] - R[1, 0, i])
        K[3, 0] = (1/3) * (R[1, 2, i] - R[2, 1, i])
        K[3, 1] = (1/3) * (R[2, 0, i] - R[0, 2, i])
        K[3, 2] = (1/3) * (R[0, 1, i] - R[1, 0, i])
        K[3, 3] = (1/3) * (R[0, 0, i] + R[1, 1, i] + R[2, 2, i])
        
        eigvals, eigvecs = np.linalg.eig(K)
        max_index = np.argmax(eigvals)
        q[i, :] = eigvecs[:, max_index]
        q[i, :] = [q[i, 3], q[i, 0], q[i, 1], q[i, 2]]
    
    return q
