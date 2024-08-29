import csv
import os
import json
import cgitb
import logging
import datetime

cgitb.enable()
logging.basicConfig(filename="todo.log", level=logging.DEBUG)

def get_todo_list():
	todo_list = []
	if not os.path.exists('todo.csv'):
		logging.debug("No todo.csv file found")
		return todo_list
	with open('todo.csv', 'r') as f:
		reader = csv.DictReader(f)
		todo_list = list(reader)
	logging.debug(f"Todo list: {todo_list}")
	return todo_list

def save_todo_list(todo_list):
	with open('todo.csv', 'w') as f:
		writer = csv.DictWriter(f, fieldnames=['item', 'done', 'date'])
		writer.writeheader()
		writer.writerows(todo_list)

def add_todo_item(todo_list, item):
	item = json.loads(item).get('item')
	if item:
		todo_list.append({
			'item': item, 'done': 'False',
			'date': datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		})
		save_todo_list(todo_list)

def delete_todo_item(todo_list, index):
	del todo_list[index]
	save_todo_list(todo_list)

def toggle_todo_item(todo_list, index):
	if todo_list[index].get('done') == 'True':
		todo_list[index]['done'] = 'False'
	else:
		todo_list[index]['done'] = 'True'
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
			item = os.read(0, int(envp['CONTENT_LENGTH'])).decode()
			if not item or item == '':
				item = envp.get('FORM_todo')
			add_todo_item(todo_list, item)
			print('HTTP/1.1 201 Created\r\n')
			return
		except Exception as e:
			logging.error(f"Error: {e}")
			print('HTTP/1.1 400 Bad Request\r\n')
			return
	elif method == 'DELETE':
		index = int(envp['PATH_INFO'].split('/')[1])
		delete_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content\r\n')
		return
	elif method == 'PUT':
		index = int(envp['PATH_INFO'].split('/')[1])
		toggle_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content\r\n')
		return
	elif method == 'GET':
		response += 'HTTP/1.1 200 OK\r\n'
	elif method == 'OPTIONS':
		response += 'HTTP/1.1 204 OK\r\n'
		response += 'Access-Control-Allow-Origin: *\r\n'
		response += 'Access-Control-Allow-Methods: GET, POST, DELETE, PUT\r\n'
		response += 'Access-Control-Allow-Headers: Content-Type\r\n'
		response += 'Access-Control-Allow-Credentials: true\r\n'
		response += 'Access-Control-Max-Age: 86400\r\n'
		response += 'Content-Length: 0\r\n'
		print(response)
		return
	else:
		print('HTTP/1.1 405 Method Not Allowed\r\n')
		return

	json_list = json.dumps(todo_list)
	logging.debug(f"JSON List: {json_list}")
	response += 'Content-Type: application/json\r\n'
	response += 'Content-Length: {}\r\n'.format(len(json_list) + 1)
	response += '\r\n'
	response += json_list
	print(response)

main()