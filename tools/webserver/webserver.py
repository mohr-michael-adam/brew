from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/", methods = ['POST'])
def index():
    request_data = request.get_json()

    print("Got the request data %s" % request_data)

    return ''

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)