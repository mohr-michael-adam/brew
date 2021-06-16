terraform {
  backend "remote" {
    organization = "mohr-brew"

    workspaces {
      name = "brew"
    }
  }
}
