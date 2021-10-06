resource "aws_s3_bucket" "brew_data_bucket" {
  bucket = "mohr-brew-data-bucket"
  acl    = "public-read"
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
