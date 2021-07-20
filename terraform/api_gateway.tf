resource "aws_apigatewayv2_api" "apigateway_lambda" {
  name          = "serverless_lambda_gw"
  protocol_type = "HTTP"
}

resource "aws_apigatewayv2_stage" "apigateway_stage_lambda" {
  api_id = aws_apigatewayv2_api.apigateway_lambda.id

  name        = "serverless_lambda_stage"
  auto_deploy = true

  access_log_settings {
    destination_arn = aws_cloudwatch_log_group.api_gw.arn

    format = jsonencode({
      requestId               = "$context.requestId"
      sourceIp                = "$context.identity.sourceIp"
      requestTime             = "$context.requestTime"
      protocol                = "$context.protocol"
      httpMethod              = "$context.httpMethod"
      resourcePath            = "$context.resourcePath"
      routeKey                = "$context.routeKey"
      status                  = "$context.status"
      responseLength          = "$context.responseLength"
      integrationErrorMessage = "$context.integrationErrorMessage"
      }
    )
  }
}

resource "aws_apigatewayv2_integration" "apigateway_integration_lambda" {
  api_id = aws_apigatewayv2_api.apigateway_lambda.id

  integration_uri    = aws_lambda_function.receiver_lambda.invoke_arn
  integration_type   = "AWS_PROXY"
  integration_method = "POST"
}

resource "aws_apigatewayv2_route" "apigateway_route_lambda" {
  api_id = aws_apigatewayv2_api.apigateway_lambda.id

  route_key = "POST /measurement"
  target    = "integrations/${aws_apigatewayv2_integration.apigateway_integration_lambda.id}"
}

resource "aws_cloudwatch_log_group" "api_gw" {
  name = "/aws/api_gw/${aws_apigatewayv2_api.apigateway_lambda.name}"

  retention_in_days = 30
}

resource "aws_lambda_permission" "lambda_permission_receiever" {
  statement_id  = "AllowExecutionFromAPIGateway"
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.receiver_lambda.function_name
  principal     = "apigateway.amazonaws.com"

  source_arn = "${aws_apigatewayv2_api.apigateway_lambda.execution_arn}/*/*"
}