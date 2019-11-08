#! /usr/bin/env python
# user interface team PV Robot
import os
import json
import sys
from flask import Flask, render_template, request, url_for, redirect

# get config info from launch string
# ex: "$python -m server.py ROS" will launch the server in ROS mode
# "$python -m server.py DEBUG" will launch the server in debug (local) mode
config = str(sys.argv[1])

if config == "ROS":
	import rospy
	import threading
	from std_msgs.msg import String
	hello_str = "test "
	
	#we need callbacks to handle data from subscribed topics
	def ros_callback(msg):
		global hello_str
		hello_str = msg.data
		print(hello_str)

	threading.Thread(target=lambda: rospy.init_node('example_node', disable_signals=True)).start()
	rospy.Subscriber('chatter', String, ros_callback)
	

if config == "DEBUG":
	pass

print(config)

# this creates the flask "App" for the server
app = Flask(__name__)

# each route here is only used for templated webpages and REST API requests
@app.route('/index.html')
def hello_world():
	global hello_str
	return render_template('index.html')
	
@app.route('/')
def return_home():
	return redirect(url_for('hello_world'))
	
@app.route('/pages/Add_Drop/add_drop.html')
def add_drop():
	return render_template('pages/Add_Drop/add_drop.html')
	
@app.route('/pages/Add_Location/add_location.html')
def add_location():
	return render_template('/pages/Add_Location/add_location.html')
	
@app.route('/pages/home_page/home.html')
def home_page():
	return render_template('pages/home_page/home.html')
	
@app.route('/pages/Map_Page/map_page.html')
def map_page():
	return render_template('pages/Map_Page/map_page.html')
	
@app.route('/pages/Dashboard/dashboard.html')
def dashboard():
	return render_template('pages/Dashboard/dashboard.html')


@app.route('/topics.json', methods = ['GET'])
def topics_get():
	global config
	if config == "ROS":
		topics = rospy.get_published_topics(namespace='/')
		return json.dumps(topics)
		
	else:
		topics = "blank"
		return topics
		
		
@app.route('/dash/connection.json', methods = ['GET'])
def get_connection_status():
	global config
	if config == "DEBUG":
		connection = "Connected"
		return json.dumps(connection)
	
	
@app.route('/dash/battery.json', methods = ['GET'])
def get_battery():
	global config
	if config == "DEBUG":
		battery = 99
		return json.dumps(battery)
			
			
@app.route('/dash/latitude.json', methods = ['GET'])
def get_latitude():
	global config
	if config == "DEBUG":
		latitude = 30.001
		return json.dumps(latitude)
			
			
@app.route('/dash/longitude.json', methods = ['GET'])
def get_longitude():
	global config
	if config == "DEBUG":
		longitude = 100.001
		return json.dumps(longitude)
			
		
@app.route('/dash/distance.json', methods = ['GET'])
def get_distance():
	global config
	if config == "DEBUG":
		distance = 10
		return json.dumps(distance)
			

@app.route('/dash/orientation.json', methods = ['GET'])
def get_orientation():
	global config
	if config == "DEBUG":
		orientation = 23
		return json.dumps(orientation)
			

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
