import csv
import os
import json
import cgitb
import logging

cgitb.enable()
logging.basicConfig(filename="todo.log", level=logging.DEBUG)

def get_todo_list():
	todo_list = []
	if not os.path.exists('todo.csv'):
		logging.debug("No todo.csv file found")
		return todo_list
	with open('todo.csv', 'r') as f:
		reader = csv.reader(f)
		todo_list = list(reader)
	logging.debug(f"Todo list: {todo_list}")
	return todo_list

def save_todo_list(todo_list):
	with open('todo.csv', 'w') as f:
		writer = csv.writer(f)
		writer.writerows(todo_list)

def add_todo_item(todo_list, item):
	item = json.loads(item).get('item')
	if item:
		todo_list.append([item, 'False'])
		save_todo_list(todo_list)

def delete_todo_item(todo_list, index):
	del todo_list[index]
	save_todo_list(todo_list)

def toggle_todo_item(todo_list, index):
	if todo_list[index][1] == 'True':
		todo_list[index][1] = 'False'
	else:
		todo_list[index][1] = 'True'
	save_todo_list(todo_list)

def main():
	envp = os.environ
	method = envp['REQUEST_METHOD']
	logging.debug(f"Environment Variables: {envp}")
	todo_list = get_todo_list()
	logging.debug(f"Method: {method}")
	response = ''
	if method == 'POST':
		try:
			logging.debug("Content-Length: {}".format(envp['CONTENT_LENGTH']))
			item = os.read(0, int(envp['CONTENT_LENGTH'])).decode()
			logging.debug(f"Item: {item}")
			add_todo_item(todo_list, item)
		except Exception as e:
			logging.error(f"Error: {e}")
			print('HTTP/1.1 400 Bad Request')
			return
		print('HTTP/1.1 201 Created')
		return
	elif method == 'DELETE':
		index = int(envp['PATH_INFO'].split('/')[1])
		delete_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content')
		return
	elif method == 'PUT':
		index = int(envp['PATH_INFO'].split('/')[1])
		toggle_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content')
		return
	elif method == 'GET':
		response += 'HTTP/1.1 200 OK\r\n'
	else:
		print('HTTP/1.1 405 Method Not Allowed')
		return

	json_list = json.dumps(todo_list)
	response += 'Content-Type: application/json\r\n'
	# print('Access-Control-Allow-Origin: *')
	# print('Access-Control-Allow-Methods: GET, POST, DELETE, PUT')
	# print('Access-Control-Allow-Headers: Content-Type')
	response += 'Content-Length: {}\r\n'.format(len(json_list))
	response += '\r\n'
	response += json_list
	print(response)

main()