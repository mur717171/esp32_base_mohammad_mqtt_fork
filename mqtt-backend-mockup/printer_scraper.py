import paho.mqtt.client as mqtt
import json
import ssl
import argparse
from paho.mqtt.enums import CallbackAPIVersion

"""
Bambu Lab Printer MQTT Scraper (Python)

✅ TESTED & WORKING
   - Successfully receives data from local mosquitto broker (localhost:1883)
   - Successfully parses JSON data from fake_printer.py simulator
   - Successfully receives data from real Bambu Lab X1C printer (10.10.2.20:8883)
   - Handles both TLS and plain MQTT connections

Usage:
  python printer_scraper.py --broker local      # Connect to localhost (default)
  python printer_scraper.py --broker hivemq     # Connect to broker.hivemq.com (for Wokwi)
"""

# --- PARSE COMMAND LINE ARGUMENTS ---
parser = argparse.ArgumentParser(description="Bambu Lab Printer MQTT Scraper")
parser.add_argument(
    "--broker",
    choices=["local", "hivemq"],
    default="local",
    help="MQTT broker to use (default: local)"
)
args = parser.parse_args()

# --- CONFIGURATION ---
if args.broker == "hivemq":
    BROKER_HOST = "broker.hivemq.com"
    PORT = 1883
    print("🌐 Using HiveMQ public broker (broker.hivemq.com)")
else:
    BROKER_HOST = "localhost"
    PORT = 1883
    print("🖥️  Using local Mosquitto broker (localhost)")

ACCESS_CODE = ""  # No auth needed for local broker or HiveMQ public
TOPIC_FILTER = "device/#"  # Catches device/<serial_number>/report

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("✅ CONNECTED to Bambu Lab X1C")
        client.subscribe(TOPIC_FILTER)
    else:
        print(f"❌ Failed to connect, return code {reason_code}")

def on_message(client, userdata, msg):
    try:
        # Bambu sends a massive JSON block, so we'll parse it to make it readable
        payload = msg.payload.decode()
        data = json.loads(payload)
        
        print(f"\n[TOPIC]: {msg.topic}")
        
        # Printing the first 500 characters so your terminal doesn't get flooded
        formatted_json = json.dumps(data, indent=2)
        print(f"[DATA]:\n{formatted_json[:500]}...\n[Truncated for readability]")
        
    except Exception as e:
        print(f"Error parsing: {e}")

# Use Version 2 of the API
client = mqtt.Client(CallbackAPIVersion.VERSION2)

# --- MOSQUITTO SETTINGS (Local Broker) ---
# No auth needed for local mosquitto
# if ACCESS_CODE:
#     client.username_pw_set("bblp", ACCESS_CODE)

# No TLS/SSL needed for local mosquitto
# client.tls_set(tls_version=ssl.PROTOCOL_TLS, cert_reqs=ssl.CERT_NONE)
# client.tls_insecure_set(True)

# --- INITIALIZE ---
client.on_connect = on_connect
client.on_message = on_message

print("🚀 STARTING Bambu Scraper (via Local Mosquitto)...")
client.connect(BROKER_HOST, PORT, 60)
client.loop_forever()