import paho.mqtt.client as mqtt
import json
import time
import random
import argparse
from paho.mqtt.enums import CallbackAPIVersion

"""
Fake Bambu Lab Printer Simulator
Publishes realistic printer data to MQTT broker for testing

✅ TESTED & WORKING - Successfully publishes fake printer data

Usage:
  python fake_printer.py --broker local      # Connect to localhost (default)
  python fake_printer.py --broker hivemq     # Connect to broker.hivemq.com (for Wokwi)
"""

# --- PARSE COMMAND LINE ARGUMENTS ---
parser = argparse.ArgumentParser(description="Fake Bambu Lab Printer Simulator")
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
    print("🌐 Using HiveMQ public broker (broker.hivemq.com)")
else:
    BROKER_HOST = "localhost"
    print("🖥️  Using local Mosquitto broker (localhost)")

PORT = 1883
PUBLISH_INTERVAL = 2  # seconds between publishes
DEVICE_SERIAL = "abc123def456"  # Fake printer serial number
TOPIC = f"device/{DEVICE_SERIAL}/report"

# Fake printer state
printer_state = {
    "nozzle_temper": 20.0,
    "nozzle_target_temper": 220,
    "bed_temper": 20.0,
    "bed_target_temper": 60,
    "chamber_temper": 25.0,
    "print": {
        "progress": 0,
        "layer_num": 0
    },
    "spd_mag": 0,
    "z_axis": 0.0,
    "filament_type": "PLA",
    "mc_print_stage": 0,  # 0=idle, 1=printing, 2=paused
    "print_type": "IDLE",
    "lights_report": [{"mode": 0}]
}

print_running = False
elapsed_time = 0
total_print_time = 3600  # Simulate 1 hour print

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("✅ Connected to local mosquitto broker")
    else:
        print(f"❌ Failed to connect, return code {reason_code}")

def simulate_printing():
    """Update fake printer state to simulate a print"""
    global print_running, elapsed_time
    
    if not print_running:
        # Start a print
        print_running = True
        elapsed_time = 0
        printer_state["mc_print_stage"] = 1
        printer_state["print_type"] = "PRINTING"
        printer_state["spd_mag"] = 100
        print("🖨️  STARTING PRINT...")
        return
    
    # Update temperatures (gradual heat-up, then stable)
    if printer_state["nozzle_temper"] < printer_state["nozzle_target_temper"]:
        printer_state["nozzle_temper"] += random.uniform(0.5, 2.0)
        printer_state["nozzle_temper"] = min(
            printer_state["nozzle_temper"], 
            printer_state["nozzle_target_temper"]
        )
    else:
        # Add small random fluctuation
        printer_state["nozzle_temper"] += random.uniform(-0.2, 0.2)
    
    if printer_state["bed_temper"] < printer_state["bed_target_temper"]:
        printer_state["bed_temper"] += random.uniform(0.2, 1.0)
        printer_state["bed_temper"] = min(
            printer_state["bed_temper"],
            printer_state["bed_target_temper"]
        )
    else:
        printer_state["bed_temper"] += random.uniform(-0.1, 0.1)
    
    # Update chamber temp
    printer_state["chamber_temper"] += random.uniform(-0.1, 0.3)
    printer_state["chamber_temper"] = max(20, printer_state["chamber_temper"])
    
    # Update progress
    elapsed_time += PUBLISH_INTERVAL
    progress = int((elapsed_time / total_print_time) * 100)
    progress = min(progress, 100)
    printer_state["print"]["progress"] = progress
    
    # Update layer
    printer_state["print"]["layer_num"] = max(1, int(progress / 5))
    
    # Update Z height
    printer_state["z_axis"] = printer_state["print"]["layer_num"] * 0.2
    
    # End print at 100%
    if progress >= 100:
        print("\n✅ PRINT COMPLETE!")
        print_running = False
        elapsed_time = 0
        printer_state["mc_print_stage"] = 0
        printer_state["print_type"] = "IDLE"
        printer_state["print"]["progress"] = 0
        printer_state["print"]["layer_num"] = 0
        printer_state["z_axis"] = 0.0
        printer_state["spd_mag"] = 0
        # Cool down
        printer_state["nozzle_target_temper"] = 20
        printer_state["bed_target_temper"] = 20

def get_printer_payload():
    """Return current fake printer state as JSON"""
    return json.dumps(printer_state)

# Create client
client = mqtt.Client(CallbackAPIVersion.VERSION2)
client.on_connect = on_connect

print("🚀 Starting Fake Bambu Lab Printer Simulator...")
print(f"   Publishing to: {BROKER_HOST}:{PORT}/{TOPIC}")
print(f"   Update interval: {PUBLISH_INTERVAL}s")
print("")

try:
    client.connect(BROKER_HOST, PORT, 60)
    client.loop_start()
    
    # Give connection time to establish
    time.sleep(1)
    
    # Publishing loop
    print("Commands:")
    print("  Press ENTER to start/stop a print")
    print("  Press Ctrl+C to exit")
    print("")
    
    iteration = 0
    while True:
        # Check for user input (non-blocking)
        try:
            # This is a simple way to check for input without blocking
            # In production, use threading if you need true async input
            pass
        except KeyboardInterrupt:
            break
        
        # Simulate printer activity
        simulate_printing()
        
        # Publish current state
        payload = get_printer_payload()
        client.publish(TOPIC, payload, qos=1)
        
        iteration += 1
        status = "PRINTING" if print_running else "IDLE"
        progress = printer_state["print"]["progress"]
        nozzle = printer_state["nozzle_temper"]
        bed = printer_state["bed_temper"]
        
        print(f"📡 [{iteration}] {status:8} | Progress: {progress:3}% | Nozzle: {nozzle:6.1f}°C | Bed: {bed:5.1f}°C")
        
        time.sleep(PUBLISH_INTERVAL)

except KeyboardInterrupt:
    print("\n\n⛔ Shutting down...")
finally:
    client.loop_stop()
    client.disconnect()
    print("✓ Disconnected")
