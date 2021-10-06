import logging
import simplejson as json
import boto3
from boto3.dynamodb.conditions import Key

logger = logging.getLogger()
logger.setLevel(logging.INFO)


def lambda_handler(event, context):
    logger.info('Event: %s', json.dumps(event))

    # when routed via API Gateway the payload changes
    if "body" in event:
        event = json.loads(event['body'])

    brew = event['brew']

    dynamodb = boto3.resource('dynamodb')

    table = dynamodb.Table('brew')
    response = table.query(
        KeyConditionExpression=Key('brew').eq(brew)
    )

    if 'ResponseMetadata' in response:
        if 'HTTPStatusCode' in response['ResponseMetadata']:
            if response['ResponseMetadata']['HTTPStatusCode'] != 200:
                return {"statusCode": response['ResponseMetadata']['HTTPStatusCode'], "body": "Failure"}

    items = response['Items']

    logger.info('Ran query for brew %s', brew)

    s3 = boto3.client('s3')
    response = s3.put_object(
        Body=json.dumps(items),
        Bucket='mohr-brew-data-bucket',
        Key=brew + '.json'
    )

    if 'ResponseMetadata' in response:
        if 'HTTPStatusCode' in response['ResponseMetadata']:
            if response['ResponseMetadata']['HTTPStatusCode'] != 200:
                return {"statusCode": response['ResponseMetadata']['HTTPStatusCode'], "body": "Failure"}

    logger.info('Uploaded brew data to %s', brew + '.json')

    return {"statusCode": 200, "body": "Success"}




