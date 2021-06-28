resource "aws_dynamodb_table" "dynamo_brew" {
  name           = "brew"
  billing_mode   = "PROVISIONED"
  read_capacity  = 20
  write_capacity = 20
  hash_key       = "brew"
  range_key      = "timestamp"

  attribute {
    name = "brew"
    type = "S"
  }

  attribute {
    name = "timestamp"
    type = "S"
  }

  # Server side encryption is still enabled but using AWS owned CMK (free tier)
  server_side_encryption {
    enabled = false
  }

  point_in_time_recovery {
    enabled = true
  }
}
