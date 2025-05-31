resource "aws_cloudwatch_event_rule" "export_brew_event_rule" {
  name        = "export-brew-event-rule"

  schedule_expression = "cron(0/15 * * * ? *)"

  is_enabled = true
}

resource "aws_cloudwatch_event_target" "export_brew_event_target" {
  arn  = aws_lambda_function.exporter_lambda.arn
  rule = aws_cloudwatch_event_rule.export_brew_event_rule.id

  input = <<EOF
{
  "brew": "Festa Brew BMR Cerveza Spring 2025"
}
EOF

}
