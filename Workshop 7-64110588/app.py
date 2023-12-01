from flask import Flask, jsonify, render_template
import requests
from paho.mqtt import client as mqtt_client

broker  = "192.168.102.253"
port = 1883

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client("server")
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


app = Flask(__name__)
server_ip = "http://192.168.102.253:4000"
@app.route("/")
def home():
    return render_template("index.html")

@app.route("/thingspeak")
def thingspeak():
    headers = {'Accept': 'application/json'}

    url = "https://thingspeak.com/channels/2353015/feed.json"
    r = requests.get(url, headers=headers)
    return r.json();

@app.route("/nodemcu/last")
def nodemcu():
    headers = {'Accept': 'application/json'}
    r = requests.get(server_ip+'/Sensors?_sort=id&_order=desc&_limit=1', headers=headers)
    return r.json();

@app.route("/nodemcu/led/<status>")
def led_publish(status):
    client.loop_start()
    result = client.publish("led", status)
    client.loop_stop()
    return "OK";

if __name__ == "__main__":
    client = connect_mqtt()
    app.run(host='0.0.0.0', debug=True)