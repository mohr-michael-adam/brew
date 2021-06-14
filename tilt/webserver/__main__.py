import argparse
import json

from flask import Flask
from flask import request

from tilt import verify_sign

app = Flask(__name__)
public_key = None


def _get_args():
    parser = argparse.ArgumentParser(description='Test the data signing')
    parser.add_argument('public_key', help='The public key')

    return parser.parse_args()


@app.route("/", methods=['POST'])
def index():
    request_data = request.get_json()

    print("Got the request data %s" % request_data)

    signature = request_data['signature']

    del request_data['signature']

    encoded_data = json.dumps(request_data).encode('utf-8')

    if verify_sign(public_key, signature.encode("utf-8"), encoded_data):
        print("Validated the signature")
    else:
        print("Unable to validate the signature")

    return ''


if __name__ == '__main__':
    args = _get_args()

    public_key = args.public_key

    app.run(host='0.0.0.0', port=5000)