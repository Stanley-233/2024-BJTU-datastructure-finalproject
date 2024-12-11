from flask import Flask, request, jsonify
import sqlite3
import os

DATABASE = 'game.db'
app = Flask(__name__)

def get_db_connection():
    conn = sqlite3.connect(DATABASE)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    if not os.path.exists(DATABASE):
        conn = sqlite3.connect(DATABASE)
        c = conn.cursor()
        c.execute('''
            CREATE TABLE IF NOT EXISTS players (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE,
                password TEXT NOT NULL
            )
        ''')
        conn.commit()
        conn.close()

@app.route('/login', methods=['POST'])
def login():
    data = request.json
    username = data.get('username')
    password = data.get('password')
    print(username, password)

    if not username or not password:
        return jsonify({'message': 'Missing username or password!'}), 400

    conn = get_db_connection()
    player = conn.execute('SELECT * FROM players WHERE name = ?', (username,)).fetchone()
    conn.close()

    if player is None:
        return jsonify({'message': 'No such user!'}), 404
    elif player['password'] != password:
        return jsonify({'message': 'Wrong password!'}), 403
    else:
        return jsonify({'message': 'Login successful!'}), 200

@app.route('/register', methods=['POST'])
def register():
    data = request.json
    username = data.get('username')
    password = data.get('password')

    if not username or not password:
        return jsonify({'message': 'Missing username or password!'}), 400

    conn = get_db_connection()
    try:
        conn.execute('INSERT INTO players (name, password) VALUES (?, ?)', (username, password))
        conn.commit()
        return jsonify({'message': 'User registered successfully!'}), 201
    except sqlite3.IntegrityError:
        return jsonify({'message': 'Username already exists!'}), 409
    finally:
        conn.close()

@app.route('/')
def hello_world():  # put application's code here
    init_db()
    return 'Hello World!'

if __name__ == '__main__':
    app.run()
