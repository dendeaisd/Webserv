import csv
import datetime
import json
import logging
import os

logging.basicConfig(filename="./cgi-bin/todo/todo.log", level=logging.DEBUG)
db = './cgi-bin/todo/todo.csv'

def get_todo_list():
	todo_list = []
	if not os.path.exists(db):
		logging.debug("No todo.csv file found")
		return todo_list
	with open(db, 'r') as f:
		reader = csv.DictReader(f)
		todo_list = list(reader)
	for item in todo_list:
		item['done'] = True if item['done'] == 'True' else False
	return todo_list

def save_todo_list(todo_list):
	with open(db, 'w') as f:
		writer = csv.DictWriter(f, fieldnames=['item', 'done', 'date'])
		writer.writeheader()
		writer.writerows(todo_list)

def add_todo_item(todo_list, item):
	item = json.loads(item).get('item')
	if item:
		todo_list.append({
			'item': item, 'done': False,
			'date': datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		})
		save_todo_list(todo_list)

def delete_todo_item(todo_list, index):
	del todo_list[index]
	save_todo_list(todo_list)

def toggle_todo_item(todo_list, index):
	if todo_list[index].get('done') == True:
		todo_list[index]['done'] = False
	else:
		todo_list[index]['done'] = True
	save_todo_list(todo_list)

def get_options_response():
	response = 'HTTP/1.1 204 OK\r\n'
	response += 'Access-Control-Allow-Origin: *\r\n'
	response += 'Access-Control-Allow-Methods: GET, POST, DELETE, PUT\r\n'
	response += 'Access-Control-Allow-Headers: Content-Type\r\n'
	response += 'Access-Control-Allow-Credentials: true\r\n'
	response += 'Access-Control-Max-Age: 86400\r\n'
	response += 'Content-Length: 0\r\n'
	return response

def handle_post_request(todo_list, envp):
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


def handle_put_request(todo_list, envp):
	if 'URL_ARG' not in envp:
		print('HTTP/1.1 400 Bad Request\r\n')
		return
	try:
		index = int(envp['URL_ARG'])
		toggle_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content\r\n')
		return
	except Exception as e:
		logging.error(f"Error: {e}")
		print('HTTP/1.1 400 Bad Request\r\n')
		return
	
def handle_delete_request(todo_list, envp):
	if 'URL_ARG' not in envp:
		print('HTTP/1.1 400 Bad Request\r\n')
		return
	try:
		index = int(envp['URL_ARG'])
		delete_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content\r\n')
		return
	except Exception as e:
		logging.error(f"Error: {e}")
		print('HTTP/1.1 400 Bad Request\r\n')
		return

def main():
	envp = os.environ
	method = envp['REQUEST_METHOD']
	todo_list = get_todo_list()
	response = ''
	if method == 'GET':
		response += 'HTTP/1.1 200 OK\r\n'
	elif method == 'POST':
		handle_post_request(todo_list, envp)
		return
	elif method == 'DELETE':
		handle_delete_request(todo_list, envp)
		return
	elif method == 'PUT':
		handle_put_request(todo_list, envp)
		return
	elif method == 'OPTIONS':
		print(get_options_response())
		return
	else:
		print('HTTP/1.1 405 Method Not Allowed\r\n')
		return
	json_list = json.dumps(todo_list)
	response += 'Content-Type: application/json\r\n'
	response += 'Content-Length: {}\r\n'.format(len(json_list) + 1)
	response += '\r\n'
	response += json_list
	print(response)

main()
