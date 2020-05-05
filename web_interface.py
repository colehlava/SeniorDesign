# web_interface.py
# Interface between backend application and web app.

import flask, cv2, datetime, threading, argparse

# Global variables
app = flask.Flask(__name__.split('.')[0])
videoFeed = cv2.VideoCapture(0)
outputFrame = None
frameLock = threading.Lock()

# index
@app.route("/", methods=['GET', 'POST'])
def index():
    # Read button value inputs from user
    if flask.request.method == 'POST':
        if flask.request.form.get('on') == 'on':
            with open('button_status.txt', 'w') as bs:
                bs.write("1")
        elif flask.request.form.get('off') == 'off':
            with open('button_status.txt', 'w') as bs:
                bs.write("0")

    # Return the rendered html file
    return flask.render_template("index.html")


# Respond to slider input from user
@app.route("/speed_slider", methods=['GET', 'POST'])
def updateSpeed():
    # Read speed slider value from user
    if flask.request.method == 'POST':
        with open('speed.txt', 'w') as s:
            s.write(flask.request.form['speed'])

    # Return the rendered html file
    return flask.render_template("index.html")


# Stream video from web cam
def streamVideo():
    global videoFeed, outputFrame, frameLock

    while True:
        # Capture frame
        ret, frame = videoFeed.read()

        # Acquire the lock, update the output frame, then release the lock
        with frameLock:
            outputFrame = frame.copy()


# Generate a jpg file of the image and format into a byte array
def generate():
    # Grab global references to the output frame and frame lock variables
    global outputFrame, frameLock

    # Loop over frames from the output stream
    while True:
        # Wait until the lock is acquired
        with frameLock:
            if outputFrame is None:
                continue

            # Encode the frame into jpg format
            (flag, encodedImage) = cv2.imencode(".jpg", outputFrame)

        # Ensure the frame was successfully encoded
        if not flag:
            continue

        # Yield a byte array of the .jpg frame
        yield(b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + bytearray(encodedImage) + b'\r\n')


# Generate video frames for web application
@app.route("/video_feed")
def video_feed():
    # Return the generated response and media type
    return flask.Response(generate(),
        mimetype = "multipart/x-mixed-replace; boundary=frame")


# main
def main():
    # Parse command line arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--ip", type=str, default="127.0.0.1", help="ip address of the device")
    ap.add_argument("-o", "--port", type=int, default="5005", help="ephemeral port number of the server (1024 to 65535)")
    args = vars(ap.parse_args())

    # Start thread to stream video
    video_thread = threading.Thread(target=streamVideo, args=())
    video_thread.start()

    # Start the flask web application
    app.run(host=args["ip"], port=args["port"], debug=True,
        threaded=True, use_reloader=False)


# Begin program
if __name__ == '__main__':
    main()

