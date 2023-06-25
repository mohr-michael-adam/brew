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
  source = "ui/index.html"
  content_type = "text/html"
  etag = filemd5("ui/index.html")
}

resource "aws_s3_bucket_object" "page_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "page.html"
  source = "ui/page.html"
  content_type = "text/html"
  etag = filemd5("ui/page.html")
}

resource "aws_s3_bucket_object" "format_css_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "format.css"
  source = "ui/format.css"
  content_type = "text/css"
  etag = filemd5("ui/format.css")
}

resource "aws_s3_bucket_object" "logic_js_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "logic.js"
  source = "ui/logic.js"
  content_type = "text/javascript"
  etag = filemd5("ui/logic.js")
}

resource "aws_s3_bucket_object" "german_pilsner_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "German Pilsner.html"
  source = "ui/German Pilsner.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "saaz_golden_ale_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Saaz Golden Ale.html"
  source = "ui/Saaz Golden Ale.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "irish_red_ale_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Irish Red Ale.html"
  source = "ui/Irish Red Ale.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "whole_hop_experiment_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Whole Hop Experiment.html"
  source = "ui/Whole Hop Experiment.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "dusseldorf_altbier_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Dusseldorf Altbier.html"
  source = "ui/Dusseldorf Altbier.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "german_pils_feb_21_2022_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "German Pils Feb 21 2022.html"
  source = "ui/German Pils Feb 21 2022.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "anchor_steam_clone_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Anchor Steam Clone.html"
  source = "ui/Anchor Steam Clone.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "german_pils_oct_11_2022_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "German Pils Oct 11 2022.html"
  source = "ui/German Pils Oct 11 2022.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "festa_brew_cream_ale_fall_2022_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Festa Brew Cream Ale Fall 2022.html"
  source = "ui/Festa Brew Cream Ale Fall 2022.html"
  content_type = "text/html"
}

resource "aws_s3_bucket_object" "festa_brew_page_ale_summer_2023_html_object" {
  bucket = aws_s3_bucket.brew_data_bucket.id
  key    = "Festa Brew Pale Ale Summer 2023.html"
  source = "ui/Festa Brew Pale Ale Summer 2023.html"
  content_type = "text/html"
}