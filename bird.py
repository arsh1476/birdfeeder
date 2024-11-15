import paho.mqtt.client as mqtt
import time
import random
from picamera import PiCamera

# MQTT settings
broker_address = "localhost"
topic_request = "birdFeeder/requestSpecies"
topic_response = "birdFeeder/speciesDetected"

# Initialize Pi Camera
camera = PiCamera()

# MQTT client setup
client = mqtt.Client("PiClient")

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with code {rc}")
    client.subscribe(topic_request)

def on_message(client, userdata, msg):
    if msg.topic == topic_request:
        print("Request for species identification received.")
        species = identify_species()
        client.publish(topic_response, str(species))
        print(f"Species {species} detected and response sent.")

def identify_species():
    # Capture image
    camera.start_preview()
    time.sleep(2)  # Camera warm-up time
    camera.capture('/home/pi/captured_image.jpg')
    camera.stop_preview()

    # Simulate species identification (replace with actual ML model if available)
    species_id = random.randint(1, 3)  # Randomly assigns a species ID for demonstration
    print(f"Species identified: Species {species_id}")
    return species_id

# Setup MQTT connection
client.on_connect = on_connect
client.on_message = on_message

client.connect(broker_address, 1883, 60)
client.loop_forever()
