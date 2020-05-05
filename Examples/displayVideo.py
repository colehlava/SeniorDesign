# File name:   displayVideo.py
# Description: Displays video feed to screen using OpenCV. Press q to quit application.

import cv2

def main():
    cap = cv2.VideoCapture(0)

    while (True):
        # Capture frame
        ret, frame = cap.read()

        # Format frame into image
        image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) # grayscale image
        #image = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV) # HSV image

        # Display the resulting frame
        cv2.imshow('frame', image)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()



if __name__ == '__main__':
    main()

