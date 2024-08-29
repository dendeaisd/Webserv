const app = {
	el: '#app',
	data: {
		todos: [],
		newTodo: ''
	},
	addTodo() {
		this.newTodo = document.querySelector('#newTodoInput').value;
		console.log(this.newTodo);
		if (this.newTodo.trim() !== '') {
			fetch('http://localhost:8080/cgi-bin/todo_app.py', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ todo: this.newTodo })
			})
				.then(response => response.json())
				.then(data => {
					this.todos.push(data);
					this.newTodo = '';
				})
				.catch(error => {
					console.error(error);
				});
		}
	},
	removeTodo(index) {
		const todoId = this.todos[index].id;
		fetch(`http://localhost:8080/cgi-bin/todo_app.py/${todoId}`, {
			method: 'DELETE'
		})
			.then(() => {
				this.todos.splice(index, 1);
			})
			.catch(error => {
				console.error(error);
			});
	},
	showTodos() {
		this.todos.forEach(todo => {
			console.log(todo.todo);
		});
	},
	mounted() {
		fetch('http://localhost:80801/cgi-bin/todo_app.py')
			.then(response => response.json())
			.then(data => {
				this.todos = data;
			})
			.catch(error => {
				console.error(error);
			});
	}
};

// Mount the app
document.addEventListener('DOMContentLoaded', () => {
	const addButton = document.querySelector('#addTodoButton');
	const removeButtons = document.querySelectorAll('.removeButton');
	// app.mounted();
	addButton.addEventListener('click', (event) => {
		event.preventDefault();
		app.addTodo();
		// app.showTodos();
	});

	removeButtons.forEach((button, index) => {
		button.addEventListener('click', () => {
			app.removeTodo(index);
		});
	});
});