import paho.mqtt.client as mqtt
import requests
import json

# สร้าง MQTT client instance
client = mqtt.Client()

# กำหนด callback function เมื่อมีข้อมูลเข้ามาใน Topic "dht11"
def on_message(client, userdata, message):
    payload = json.loads(message.payload.decode())  # แปลงข้อมูลเป็น JSON
    print(f"Received message: {payload} from topic {message.topic}")
    
    # ทำ HTTP POST request ไปยัง URL โดยส่งข้อมูลในรูปแบบ JSON
    url = "http://192.168.4.253:3000/mqtt_data"
    headers = {'Content-Type': 'application/json'}
    response = requests.post(url, json=payload, headers=headers)  # ใช้ json= แทน data=
    print(response.text)  # แสดงผลลัพธ์จากการส่งข้อมูล

# กำหนด callback function เมื่อเชื่อมต่อ MQTT broker สำเร็จ
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribe เข้า Topic "dht11" เมื่อเชื่อมต่อสำเร็จ
    client.subscribe("dht11")

# กำหนด callback function สำหรับการรับข้อความ
client.on_message = on_message
client.on_connect = on_connect

# เชื่อมต่อไปยัง MQTT broker
client.connect("192.168.4.253", 1883, 60)

# เริ่มการทำงานของ MQTT client ในโหมด loop สำหรับรับข้อมูล
client.loop_forever()
