resource "aws_s3_bucket" "brew_data_bucket" {
  bucket = "mohr-brew-data-bucket"
  acl    = "public-read"

  website {
    index_document = "index.html"
  }

  cors_rule {
    allowed_headers = ["*"]
    allowed_methods = ["GET", "PUT", "POST"]
    allowed_origins = ["*"]
    expose_headers  = ["ETag"]
    max_age_seconds = 3000
  }
}

resource "aws_s3_bucket_policy" "brew_data_bucket_public_read_policy" {
  bucket = aws_s3_bucket.brew_data_bucket.id

  policy = jsonencode({
    "Version": "2012-10-17",
    "Statement": [
        {
            "Sid": "PublicReadGetObject",
            "Effect": "Allow",
            "Principal": "*",
            "Action": [
                "s3:GetObject"
            ],
            "Resource": [
                "arn:aws:s3:::mohr-brew-data-bucket/*"
            ]
        }
    ]
  })
}

resource "aws_s3_bucket_object" "index_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "index.html"
  source = "bin/index.html"
  content_type = "text/html"
}
