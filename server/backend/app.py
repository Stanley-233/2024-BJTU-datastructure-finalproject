import datetime

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
    # print(username, password)

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

@app.route('/seed', methods=['GET'])
def seed():
    # 获取当前日期
    current_date = datetime.datetime.now().date()
    # 将日期转换为字符串
    date_str = current_date.strftime('%Y-%m-%d')
    # 生成种子
    date_seed = hash(date_str)
    # 返回种子作为 JSON 响应
    return jsonify(seed=date_seed)

@app.route('/put_rank', methods=['POST'])
def put_rank():
    data = request.json
    username = data.get('username')
    # 将 score 和 time 从字符串转换为整数
    try:
        score = int(data.get('score'))
        time = int(data.get('time'))
    except (ValueError, TypeError):
        return jsonify({'message': 'Score and time must be integers!'}), 400

    if not username or not score or not time:
        return jsonify({'message': 'Missing Information!'}), 400

    current_date = datetime.datetime.now().date().strftime('%Y-%m-%d')

    conn = get_db_connection()
    cursor = conn.cursor()

    # 创建以当前日期为名的表（如果不存在）
    create_table_query = f'''
    CREATE TABLE IF NOT EXISTS "{current_date}" (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL,
        score INTEGER NOT NULL,
        time INTEGER NOT NULL
    )
    '''
    cursor.execute(create_table_query)

    try:
        # 查询当前用户的记录
        cursor.execute(f'SELECT score, time FROM "{current_date}" WHERE username = ?', (username,))
        existing_record = cursor.fetchone()
    except sqlite3.Error:
        # 捕捉数据库错误，如表不存在
        return jsonify({'message': 'Table not found!'}), 404

    if existing_record:
        existing_score, existing_time = existing_record
        # 只在新的 score 更高或新的 time 更大时更新
        if score > existing_score or (score == existing_score and time > existing_time):
            update_query = f'''
            UPDATE "{current_date}" 
            SET score = ?, time = ? 
            WHERE username = ?
            '''
            cursor.execute(update_query, (score, time, username))
            conn.commit()
            message = 'Rank updated successfully!'
        else:
            message = 'Existing record is better; no update needed.'
            return jsonify({'message': message}), 200
    else:
        # 插入新记录
        insert_query = f'''
        INSERT INTO "{current_date}" (username, score, time) 
        VALUES (?, ?, ?)
        '''
        cursor.execute(insert_query, (username, score, time))
        conn.commit()
        message = 'Rank added successfully!'

    conn.close()

    return jsonify({'message': message}), 201

@app.route('/get_rank', methods=['POST'])
def get_rank():
    data = request.json
    username = data.get('username')

    if not username:
        return jsonify({'message': 'Missing username!'}), 400

    current_date = datetime.datetime.now().date().strftime('%Y-%m-%d')
    conn = get_db_connection()
    cursor = conn.cursor()

    # 查询当前用户的记录
    try:
        # 查询当前用户的记录
        cursor.execute(f'SELECT score, time FROM "{current_date}" WHERE username = ?', (username,))
        user_record = cursor.fetchone()
    except sqlite3.Error:
        # 捕捉数据库错误，如表不存在
        return jsonify({'message': 'Table not found!'}), 404

    # 如果没有记录，返回201
    if not user_record:
        return jsonify({
            'message': 'No record found!',
        }), 201

    # 查询当前日期表中的所有用户，并按分数降序排序
    cursor.execute(f'''
        SELECT username, score, time FROM "{current_date}"
        ORDER BY score DESC, time ASC
    ''')
    all_users = cursor.fetchall()

    # 计算当前用户排名
    rank = next((index + 1 for index, row in enumerate(all_users) if row['username'] == username), None)

    # 获取前三名用户
    top_users = [{'username': row['username'], 'score': row['score'], 'time': row['time']} for row in all_users[:3]]

    conn.close()

    return jsonify({
        'message': 'Record found!',
        'username': username,
        'rank': rank,
        'top_users': top_users
    }), 200

@app.route('/')
def hello_world():  # put application's code here
    init_db()
    return 'Hello World!'

if __name__ == '__main__':
    app.run()
