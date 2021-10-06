resource "aws_lambda_function" "receiver_lambda" {
  filename      = "bin/receiver-${var.receiver_version}.zip"
  function_name = "receiver"
  role          = aws_iam_role.lambda_ex.arn
  handler       = "receiver.lambda_handler"

  source_code_hash = filebase64sha256("bin/receiver-${var.receiver_version}.zip")

  runtime = "python3.7"
}

resource "aws_lambda_function" "exporter_lambda" {
  filename      = "bin/exporter-${var.exporter_version}.zip"
  function_name = "exporter"
  role          = aws_iam_role.lambda_ex.arn
  handler       = "exporter.lambda_handler"

  source_code_hash = filebase64sha256("bin/exporter-${var.exporter_version}.zip")

  runtime = "python3.7"

  timeout = 60
}