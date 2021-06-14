import argparse
import json
from tilt import sign_data
from tilt import verify_sign

data = {'key1': 'value1'}
encoded_date = json.dumps(data).encode('utf-8')


def _get_args():
    parser = argparse.ArgumentParser(description='Test the data signing')
    parser.add_argument('private_key', help='The private key file')
    parser.add_argument('public_key', help='The public key')

    return parser.parse_args()


if __name__ == '__main__':
    args = _get_args()
    signature = sign_data(args.private_key, encoded_date)

    print("Signature %s" % signature)

    if verify_sign(args.public_key, signature, encoded_date):
        print("Signature verified")
    else:
        print("Who are you?!")
