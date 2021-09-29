resource "aws_s3_bucket" "brew_data_bucket" {
  bucket = "mohr-brew-data-bucket"
  acl    = "private"
}