#!/usr/bin/env ruby
# encoding: utf-8

require 'net/http'
require 'json'
require "open-uri"

url = URI.parse('http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/aws-template-resource-type-ref.html')
req = Net::HTTP::Get.new(url.to_s)
res = Net::HTTP.start(url.host, url.port) {|http|
  http.request(req)
}

pages = []

res.body.scan(/<li><a href="(.+?)">(.+?)<\/a><\/li>/) { |w| 

	link = w[0]
	name = w[1]
	pages << { :link => link, :name => name }

}

pages.each do |page|

	#<meta name="description" content="Creates an Auto Scaling group by using the AWS::AutoScaling::AutoScalingGroup resource.">

	url = URI.parse("http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/#{page[:link]}")
	req = Net::HTTP::Get.new(url.to_s)
	res = Net::HTTP.start(url.host, url.port) {|http|
	  http.request(req)
	}

	puts "Reading docs for #{page[:name]}"
	res.body.scan(/<meta name="description" content="(.+?)">/) { |w| 
		page[:description] = w[0]
	}

	page[:properties] = []



	res.body.scan(/<dt><a name=".+?"><\/a><span class="term">(.+?)<\/span><\/dt><dd><p>.+?<\/p><p><span class="emphasis"><em>Required<\/em><\/span>: (.+?)<\/p><p><span class="emphasis"><em>Type<\/em><\/span>: (.+?)<\/p><p><span class="emphasis"><em>Update requires<\/em><\/span>: <a class="link" href=".+?">(.+?)<\/a><\/p>/m) { |w| 

		prop = {
			:name => w[0],
			:required => w[1],
			:type => w[2].gsub(/(<[^>]*>)|\n|\t/s) {""}.strip ,
			:update => w[3]
		}

		page[:properties] << prop
	}

	shortname = page[:name].split("::").last

	paramlist = []
	page[:properties].each do |prop|
		paramlist << "\n\tstring #{prop[:name]}"
	end
	params = paramlist.join(", ")


	arglist = []
	page[:properties].each do |prop|
		arglist << "\n\t\t#{prop[:name]} = #{prop[:name]}"
	end
	arguments = arglist.join(", ")


	File.open("res/#{shortname}.cfl","w") do |f|
	  f.write(
<<-ENDOFFILE
---
#{shortname}.cfl
#{page[:description]}
---

STACK #{shortname} (#{params})
{
	MAKE #{page[:name]} Resource (#{arguments})

	RETURN (Resource = Resource)
}

ENDOFFILE
	  	)
	end
end