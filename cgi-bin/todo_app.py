import csv
import os
import json
import cgitb

cgitb.enable()

def get_todo_list():
	with open('todo.csv', 'r') as f:
		reader = csv.reader(f)
		todo_list = list(reader)
	return todo_list

def save_todo_list(todo_list):
	with open('todo.csv', 'w') as f:
		writer = csv.writer(f)
		writer.writerows(todo_list)

def add_todo_item(todo_list, item):
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
	todo_list = get_todo_list()

	if method == 'POST':
		item = os.read(0, int(envp['CONTENT_LENGTH'])).decode()
		add_todo_item(todo_list, item)
		print('HTTP/1.1 201 Created')
	elif method == 'DELETE':
		index = int(envp['PATH_INFO'].split('/')[1])
		delete_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content')
	elif method == 'PUT':
		index = int(envp['PATH_INFO'].split('/')[1])
		toggle_todo_item(todo_list, index)
		print('HTTP/1.1 204 No Content')
	else:
		print('HTTP/1.1 200 OK')

	print('Content-Type: application/json')
	print()
	print(json.dumps(todo_list))

main()