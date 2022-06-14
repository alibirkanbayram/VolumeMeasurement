# ━━━━━━━━━━━━━━━━━━━━━ Library ━━━━━━━━━━━━━━━━━━━━━
import time,pyrebase,RPi.GPIO as GPIO,numpy as np
from object_detector import *


# ━━━━━━━━━━━━━━━━━━━━━ Firebase definition ━━━━━━━━━━━━━━━━━━━━━
config = {
  "apiKey": "api_key",
  "authDomain": "auth_domain",
  "databaseURL": "database_url",
  "storageBucket": "storage"
}
firebase = pyrebase.initialize_app(config)
db = firebase.database()

# ━━━━━━━━━━━━━━━━━━━━━ Variable definition ━━━━━━━━━━━━━━━━━━━━━
company,path,machineID,errorcode = "",db,"machineID","200" # Machine ID definitions
hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
TRIG_Pin,ECHO_Pin=6,5 # Pin definition

# Load Aruco detector
parameters = cv2.aruco.DetectorParameters_create()
aruco_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_5X5_50)


# Load Object Detector
detector = HomogeneousBgDetector()

# Load Cap
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)


# ━━━━━━━━━━━━━━━━━━━━━ Area Measurement function ━━━━━━━━━━━━━━━━━━━━━
def areaMeasure(): # Area Measurement
    _, img = cap.read()

    # Get Aruco marker
    corners, _, _ = cv2.aruco.detectMarkers(img, aruco_dict, parameters=parameters)
    if corners:

        # Draw polygon around the marker
        int_corners = np.int0(corners)
        cv2.polylines(img, int_corners, True, (0, 255, 0), 5)

        # Aruco Perimeter
        aruco_perimeter = cv2.arcLength(corners[0], True)

        # Pixel to cm ratio
        pixel_cm_ratio = aruco_perimeter / 20

        contours = detector.detect_objects(img)

        # Draw objects boundaries
        for cnt in contours:
            # Get rect
            rect = cv2.minAreaRect(cnt)
            (x, y), (w, h), angle = rect

            # Get Width and Height of the Objects by applying the Ratio pixel to cm
            object_width = w / pixel_cm_ratio
            object_height = h / pixel_cm_ratio

            # Display rectangle
            box = cv2.boxPoints(rect)
            box = np.int0(box)

            cv2.circle(img, (int(x), int(y)), 5, (0, 0, 255), -1)
            cv2.polylines(img, [box], True, (255, 0, 0), 2)
            cv2.putText(img, "Width {} cm".format(round(object_width, 1)), (int(x - 100), int(y - 20)), cv2.FONT_HERSHEY_PLAIN, 2, (100, 200, 0), 2)
            cv2.putText(img, "Height {} cm".format(round(object_height, 1)), (int(x - 100), int(y + 15)), cv2.FONT_HERSHEY_PLAIN, 2, (100, 200, 0), 2)
            cv2.imshow("Image", img)
            key = cv2.waitKey(1)
            if key == 27:
                break
            width_mm, height_mm= format(round(object_width, 1)), (int(x - 100), int(y - 20)), cv2.FONT_HERSHEY_PLAIN, 2, (100, 200, 0), format(round(object_height, 1)), (int(x - 100), int(y + 15)), cv2.FONT_HERSHEY_PLAIN, 2, (100, 200, 0), 2
        return width_mm*height_mm

# ━━━━━━━━━━━━━━━━━━━━━ Distance function ━━━━━━━━━━━━━━━━━━━━━
def distance(): # Distance
    #global path
    GPIO.output(TRIG_Pin, False)
    time.sleep(2)
    GPIO.output(TRIG_Pin, True)
    time.sleep(0.00001)
    GPIO.output(TRIG_Pin, False)
    while GPIO.input(ECHO_Pin)==0:
        pulse_start = time.time()
    while GPIO.input(ECHO_Pin)==1:
        pulse_end = time.time()
    pulse_duration = pulse_end-pulse_start
    distance = pulse_duration*17150
    distance = round(distance,2)
    return distance

def volumeMeasure():
    distanceValue=distance()
    if(distance<=40):
        return distance()*areaMeasure

# ━━━━━━━━━━━━━━━━━━━━━ Setup function ━━━━━━━━━━━━━━━━━━━━━
def setup(): #Setup
    global db,company,path,machineID,errorcode
    global hourNow,tempHour
    global TRIG_Pin,ECHO_Pin
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    company=path.child("SETUP").child(machineID).get().val()
    print(company)
    path=db.child("COMPANY").child(company).child(machineID)
    path.child("INFO").update({"ERROR_CODE": errorcode,})
    GPIO.setup(ECHO_Pin, GPIO.IN)
    GPIO.setup(TRIG_Pin, GPIO.OUT)
    print("Setup ok")

# ━━━━━━━━━━━━━━━━━━━━━ Loop function ━━━━━━━━━━━━━━━━━━━━━
def loop(): # Loop
    global db,company,path,machineID,errorcode
    global hourNow,tempHour
    global TRIG_Pin,ECHO_Pin
    hourNow,tempHour= datetime.now().hour, datetime.now().hour # Now time
    print(hourNow)

# ━━━━━━━━━━━━━━━━━━━━━ Main Code ━━━━━━━━━━━━━━━━━━━━━
if __name__ == "__main__": # Main funtion
    setup() # Setup worked
    while True: # Loop started
        loop()