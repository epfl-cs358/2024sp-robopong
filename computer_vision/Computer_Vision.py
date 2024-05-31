from collections import deque
import cv2.aruco as aruco
import numpy as np
import imutils
import serial
import time
import cv2

# Constants
board_bottom_limit = 450
ball_radius_lower = 5
move_urgent_time = 0.35
paddle_min_area = 10
board_top_limit = 41

# Ball and paddle color ranges
redRange = [(160, 50, 50), (180, 255, 255)] # paddles
blueRange = [(40, 150, 200), (100, 255, 255)] # ball


paddle0_pts = deque(maxlen=64)
paddle1_pts = deque(maxlen=64)
ball_pts = deque(maxlen=64)
transformation_matrix = None
pixel_to_motor_coords = 0
position_sent = False
direction = -1
board_center = 0
goal_sent = False
width = 0
height = 0

def main():
    ser = serial.Serial('COM4', 115200)
    cap = cv2.VideoCapture(1, cv2.CAP_DSHOW) 
    frame = camera_on(cap)
    if frame is None:
        print("Problem with camera initialization.")
        return
    calibrate_camera(cap, ser)
    calibrate_paddles(cap, ser)
    track_ball_and_paddles(cap, ser)
    ser.close()
   
def camera_on(cap):
    global width, height, board_center, pixel_to_motor_coords
    time.sleep(2.0)
    if not cap.isOpened():
        return None
    ret, frame = cap.read()
    if not ret:
        return None
    height, width = frame.shape[:2]
    board_center = width // 2
    pixel_to_motor_coords = 1023 // height
    return frame

def calibrate_camera(cap, ser):
    motor0_moved = False
    motor1_moved = False
    time0 = 0
    time1 = 0
    check_again = 0

    global transformation_matrix
    # Define destination points (corners of the image)
    dst_points = np.float32([
        [0, 0],                # Upper-left corner
        [width - 1, 0],        # Upper-right corner
        [width - 1, height - 1],# Lower-right corner
        [0, height - 1]        # Lower-left corner
    ])

    # Last known positions of markers, indexed by their ID
    last_known_positions = {1: None, 2: None, 3: None, 4: None}
    aruco_dict = aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        corners, ids, _ = aruco.detectMarkers(gray, aruco_dict)
        aruco.drawDetectedMarkers(frame, corners, ids)
        if ids is not None:
            for id_array, corner in zip(ids, corners):
                id = id_array[0]
                if id in last_known_positions:
                    last_known_positions[id] = corner[0][0]
        
        if all(last_known_positions[id] is not None for id in last_known_positions):
            src_points = np.float32([
                last_known_positions[1],
                last_known_positions[2],
                last_known_positions[3],
                last_known_positions[4]
            ])
            transformation_matrix = create_transformation_matrix(src_points, dst_points)
            transformed_frame = cv2.warpPerspective(frame, transformation_matrix, (width, height))
            cv2.imshow("frame", transformed_frame)
            break
        else:
            if check_again >= 30 :
                if last_known_positions[1] is None and not motor0_moved:
                    while time0 < 100:
                        ser.write(f"cal 0 down\n".encode())
                        time0 += 1
                    motor0_moved = True
                if last_known_positions[4] is None and not motor0_moved:
                    while time0 < 100:
                        ser.write(f"cal 0 up\n".encode())
                        time0 += 1
                    motor0_moved = True
                if last_known_positions[3] is None and not motor1_moved:
                    while time1 < 100:
                        ser.write(f"cal 1 down\n".encode())
                        time1 += 1
                    motor1_moved = True
                if last_known_positions[2] is None and not motor1_moved:
                    while time1 < 100:
                        ser.write(f"cal 1 up\n".encode())
                        time1 += 1
                    motor1_moved = True
            check_again += 1

        cv2.imshow("Original", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    if cv2.getWindowProperty('Original', cv2.WND_PROP_VISIBLE) >= 1:
        cv2.destroyAllWindows()

def calibrate_paddles(cap, ser):
    paddle0_up = False
    paddle0_down = False
    paddle0_done = False
    paddle1_up = False
    paddle1_down = False
    paddle1_done = False
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        if transformation_matrix is not None:
            frame = cv2.warpPerspective(frame, transformation_matrix, (width, height))
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
        full_frame_paddle_mask = cv2.inRange(hsv, redRange[0], redRange[1])
        full_frame_paddle_mask = cv2.erode(full_frame_paddle_mask, None, iterations=2)
        full_frame_paddle_mask = cv2.dilate(full_frame_paddle_mask, None, iterations=2)
        left_paddle_mask = full_frame_paddle_mask[:, :board_center]
        left_paddle_cnts = cv2.findContours(left_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        left_paddle_cnts = imutils.grab_contours(left_paddle_cnts)
        right_paddle_mask = full_frame_paddle_mask[:, board_center:]
        right_paddle_cnts = cv2.findContours(right_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        right_paddle_cnts = imutils.grab_contours(right_paddle_cnts)
        left_paddle_center = None
        right_paddle_center = None
        full_frame_paddle_mask = cv2.inRange(hsv, redRange[0], redRange[1])
        full_frame_paddle_mask = cv2.erode(full_frame_paddle_mask, None, iterations=2)
        full_frame_paddle_mask = cv2.dilate(full_frame_paddle_mask, None, iterations=2)
        left_paddle_mask = full_frame_paddle_mask[:, :board_center]
        left_paddle_cnts = cv2.findContours(left_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        left_paddle_cnts = imutils.grab_contours(left_paddle_cnts)
        right_paddle_mask = full_frame_paddle_mask[:, board_center:]
        right_paddle_cnts = cv2.findContours(right_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        right_paddle_cnts = imutils.grab_contours(right_paddle_cnts)
        left_paddle_center = identify_triangle(left_paddle_cnts, frame, True)
        right_paddle_center = identify_triangle(right_paddle_cnts, frame, False)
        if right_paddle_center is not None:
            paddle1_pts.appendleft(right_paddle_center)
        if left_paddle_center is not None:
            paddle0_pts.appendleft(left_paddle_center)
        if len(paddle0_pts)>0 and not paddle0_done:
            if paddle0_pts[0][1] > board_top_limit and not paddle0_up:
                ser.write(f"cal 0 up\n".encode())
            elif paddle0_pts[0][1] <= board_top_limit and not paddle0_up:
                ser.write(f"cal 0 done up\n".encode())
                paddle0_up = True
            elif paddle0_pts[0][1] < board_bottom_limit and not paddle0_down:
                ser.write(f"cal 0 down\n".encode())
            elif paddle0_pts[0][1] >= board_bottom_limit and not paddle0_down:
                ser.write(f"cal 0 done down\n".encode())
                paddle0_down = True
        if len(paddle1_pts)>0 and not paddle1_done:
            if paddle1_pts[0][1] > board_top_limit and not paddle1_down:
                ser.write(f"cal 1 down\n".encode())
            elif paddle1_pts[0][1] <= board_top_limit and not paddle1_down:
                ser.write(f"cal 1 done down\n".encode())
                paddle1_down = True
            elif paddle1_pts[0][1] < board_bottom_limit and not paddle1_up:
                ser.write(f"cal 1 up\n".encode())
            elif paddle1_pts[0][1] >= board_bottom_limit and not paddle1_up:
                ser.write(f"cal 1 done up\n".encode())
                paddle1_up = True
        paddle0_done = paddle0_up and paddle0_down
        paddle1_done = paddle1_up and paddle1_down
        if paddle0_done and paddle1_done:
            break
        cv2.imshow("frame", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break   

def track_ball_and_paddles(cap, ser):
    global goal_sent
    ball_missing_count = 0
    missing_threshold = 3 
    ball_visible = False
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        if transformation_matrix is not None:
            frame = cv2.warpPerspective(frame, transformation_matrix, (width, height))
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
        ball_mask = cv2.inRange(hsv, blueRange[0], blueRange[1])
        ball_mask = cv2.erode(ball_mask, None, iterations=2)
        ball_mask = cv2.dilate(ball_mask, None, iterations=2)
        ball_cnts = cv2.findContours(ball_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        ball_cnts = imutils.grab_contours(ball_cnts)
        ball_center = None
        full_frame_paddle_mask = cv2.inRange(hsv, redRange[0], redRange[1])
        full_frame_paddle_mask = cv2.erode(full_frame_paddle_mask, None, iterations=2)
        full_frame_paddle_mask = cv2.dilate(full_frame_paddle_mask, None, iterations=2)
        left_paddle_mask = full_frame_paddle_mask[:, :board_center]
        left_paddle_cnts = cv2.findContours(left_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        left_paddle_cnts = imutils.grab_contours(left_paddle_cnts)
        right_paddle_mask = full_frame_paddle_mask[:, board_center:]
        right_paddle_cnts = cv2.findContours(right_paddle_mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        right_paddle_cnts = imutils.grab_contours(right_paddle_cnts)
        left_paddle_center = identify_triangle(left_paddle_cnts, frame, True)
        right_paddle_center = identify_triangle(right_paddle_cnts, frame, False)
        if len(ball_cnts) > 0:
            ball_c = max(ball_cnts, key=cv2.contourArea)
            ((ball_x, ball_y), ball_radius) = cv2.minEnclosingCircle(ball_c)		
            if ball_radius > ball_radius_lower:
                M = cv2.moments(ball_c)
                ball_center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
                cv2.circle(frame, (int(ball_x), int(ball_y)), int(ball_radius),
                    (0, 255, 255), 2)
                cv2.circle(frame, ball_center, 5, (0, 0, 255), -1) 
                ball_missing_count = 0
                ball_visible = True
            else:
                ball_missing_count += 1
        else:
            ball_missing_count += 1
        if ball_missing_count >= missing_threshold:
            ball_visible = False
            ball_missing_count = missing_threshold

        if ball_center is not None : 
            ball_pts.appendleft((time.time(), ball_center))
            goal_sent = False
        if left_paddle_center is not None:
            paddle0_pts.appendleft(left_paddle_center)
        if right_paddle_center is not None:
            paddle1_pts.appendleft(right_paddle_center)
        if len(ball_pts)>1 and ball_visible:
            predict_and_draw_trajectory(frame, ball_pts, ser, paddle0_pts, paddle1_pts)
        if not goal_sent and ball_missing_count >= missing_threshold:
            goal(ball_pts, ser)
        cv2.imshow("frame", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cap.release()
    cv2.destroyAllWindows()

def predict_and_draw_trajectory(frame, ball_pts, ser, paddle0_pts, paddle1_pts):
    global direction, position_sent
    corner_marge = 50
    filtered_pts = []
    oscillation_amplitude = 60  
    oscillation_duration = 0.5
    oscillation_step_time = 0.2
    if len(ball_pts) < 2:
        return
    last_time, last_pos = ball_pts[0]
    filtered_pts.append((last_time, last_pos))
    for times, pos in ball_pts:
        if last_time - times >= 0.1:
            filtered_pts.append((times, pos))
            last_time = times
    if len(filtered_pts) < 2:
        return
    (time2, (x2, y2)), (time1, (x1, y1)) = filtered_pts[0], filtered_pts[1]
    dx = x2 - x1
    dy = y2 - y1

    dt = time2 - time1
    
    if (dx < 0 and direction == 1) or (dx > 0 and direction == 0):
        direction = 1 if dx > 0 else 0
        position_sent = False
        ball_pts.clear()
        return
    elif (direction == -1):
        direction = 1 if dx > 0 else 0
    vx = dx / dt
    vy = dy / dt
    if vx == 0 and vy == 0:
        return
    t_left = t_right = t_top = t_bottom = float('inf')
    if vx != 0:
        if vx > 0:
            t_right = (width - x2) / vx
        else:
            t_left = -x2 / vx
    if vy != 0:
        if vy > 0:
            t_bottom = (height - y2) / vy
        else:
            t_top = -y2 / vy
    t_min = min(filter(lambda t: t > 0, [t_left, t_right, t_top, t_bottom]))
    final_x = x2 + int(vx * t_min)
    final_y = y2 + int(vy * t_min)
    final_y = max(0, min(height, final_y))
    if (dt == 0 or (abs(dx) < 5) or (abs(dy)< 5)):
        final_y = last_pos[1]
        final_x = last_pos[0]
        t_min = 0
        direction = 1 if final_x > 300 else 0
    cv2.line(frame, (x2, y2), (final_x, final_y), (255, 0, 0), 2)
    cv2.imshow('frame', frame)
    cv2.waitKey(1) 
    paddle_pts = paddle0_pts if direction == 0 else paddle1_pts
    if len(paddle_pts) == 0:
        return 
    last_paddle_y = paddle_pts[0][1]
    target_position = final_y if direction == 1 else height - final_y
    target_position *= pixel_to_motor_coords
    if not position_sent and t_min <= move_urgent_time and (final_x <= 0 + corner_marge or final_x >= width - corner_marge):
        start_oscillation_time = time.time()
        end_oscillation_time = start_oscillation_time + oscillation_duration
        current_time = start_oscillation_time
        toggle = True
        current_motor_position = last_paddle_y * pixel_to_motor_coords if direction == 1 else (height - last_paddle_y)*pixel_to_motor_coords
        beyond_ball_position = (target_position + oscillation_amplitude) if (current_motor_position < target_position) else (target_position - oscillation_amplitude)
        before_ball_position = (target_position - oscillation_amplitude) if (current_motor_position < target_position) else (target_position + oscillation_amplitude)
        while current_time <= end_oscillation_time:
            if toggle:
                new_position = beyond_ball_position
            else:
                new_position = before_ball_position 
            new_position = int(check_position(new_position))
            ser.write(f"game {direction} {new_position}\n".encode())
            time.sleep(oscillation_step_time)
            toggle = not toggle
            current_time = time.time()
        position_sent = True

def goal(ball_pts, ser):
    global goal_sent
    goal_threshold = 200
    if len(ball_pts) >= 2:
        last_position = ball_pts[0][1]
        second_last_position = ball_pts[1][1]
        dx = last_position[0] - second_last_position[0]
        if dx < 0 and last_position[0] <= goal_threshold:  # Moving towards paddle 1
            ser.write("goal 1\n".encode())
            print("goal 0")
            goal_sent = True
        elif dx > 0 and last_position[0] >= width - goal_threshold:  # Moving towards paddle 2
            ser.write("goal 0\n".encode())
            print("goal 1")
            goal_sent = True

def identify_triangle(contours, frame, isleft):
    best_center = None
    largest_area = 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area < paddle_min_area:
            continue
        peri = cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, 0.07 * peri, True)
        if 3 <= len(approx) <= 5:
            if area > largest_area:
                largest_area = area
                M = cv2.moments(cnt)
                if M["m00"] != 0:
                    cx = int(M["m10"] / M["m00"])
                    cy = int(M["m01"] / M["m00"])
                    best_center = (cx, cy)
                    if not isleft:
                        adjusted_cnt = cnt + np.array([[board_center, 0]])
                        adjusted_cx = cx + board_center
                    else:
                        adjusted_cnt = cnt
                        adjusted_cx = cx
    if best_center:
        cv2.drawContours(frame, [adjusted_cnt], -1, (0, 255, 0), 2)
        cv2.circle(frame, (adjusted_cx, cy), 5, (0, 0, 255), -1)
    return best_center

def create_transformation_matrix(src_points, dst_points):
    """Create a perspective transformation matrix from src_points to dst_points."""
    return cv2.getPerspectiveTransform(src_points, dst_points)

def check_position(position):
    if position < 0:
        return 0
    elif position > 1023:
        return 1023
    return position


main()
