import logging
import json

from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA256
from base64 import b64decode

logger = logging.getLogger()
logger.setLevel(logging.INFO)

public_key_path = "sign_pub.key"


def verify_sign(public_key, signature, data):
    pub_key = open(public_key, "r").read()
    rsakey = RSA.importKey(pub_key)
    signer = PKCS1_v1_5.new(rsakey)
    digest = SHA256.new()
    digest.update(data)
    if signer.verify(digest, b64decode(signature)):
        return True
    return False


def lambda_handler(event, context):
    logger.info('Event: %s', event)

    signature = event['signature']

    del event['signature']

    encoded_data = json.dumps(event).encode('utf-8')

    if verify_sign(public_key_path, signature.encode('utf-8'), encoded_data):
        logger.info('Validated the signature')
    else:
        logger.warning('Unable to validate the signature')

    response = {}
    return response
