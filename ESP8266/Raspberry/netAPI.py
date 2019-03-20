#!flask/bin/python
import json
from flask import Flask
from flask import request
from flask import jsonify

app = Flask(__name__)

@app.route('/getreading')
def getreading():
	data = {}
	f = open (".net_measurement.txt", "r")
	readings_str = f.readline()
        if readings_str == "off":
            data['status'] = "off"
        else:
	    json_readings = readings_str.split()
            data['status'] = "on"
	    data['inst_pw'] = json_readings[0]
	    data['energy'] = json_readings[1]
	json_data = json.dumps(data)
	return json_data

app.run(host='192.168.88.215', port=5000)
