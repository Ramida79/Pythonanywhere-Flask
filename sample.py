
# A very simple Flask Hello World app for you to get started with...

from flask import Flask, redirect, render_template, request, jsonify, abort, flash
from flask_sqlalchemy import SQLAlchemy
from wtforms import TextField, IntegerField, TextAreaField, SubmitField, RadioField, SelectField
from sqlalchemy import desc ,asc
from flask_marshmallow import Marshmallow
from marshmallow import Schema, fields, pprint
from datetime import datetime, timedelta
import  os
from os.path import isfile, join
from os import listdir
import json
from io import StringIO
from werkzeug.wrappers import Response
import itertools
import random
import string


app = Flask(__name__)
app.secret_key = 'development key'

SQLALCHEMY_DATABASE_URI = "mysql+mysqlconnector://{username}:{password}@{hostname}/{databasename}".format(
    username="NICK",
    password="alamakota", # database passowrd hidden
    hostname="IPZ.mysql.pythonanywhere-services.com",
    databasename="NICK$baza",
)
app.config["SQLALCHEMY_DATABASE_URI"] = SQLALCHEMY_DATABASE_URI
app.config["SQLALCHEMY_POOL_RECYCLE"] = 299 # connection timeouts
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False # no warning disruptions

db = SQLAlchemy(app)
ma = Marshmallow(app)


class Users(db.Model):

    __tablename__ = "users"
    id = db.Column(db.Integer, primary_key=True)
    full_name = db.Column(db.String(4096))
    created_at = db.Column(db.DATETIME)
    country_code =db.Column(db.Integer)


    def __init__(self, full_name, created_at, country_code):
        self.full_name = full_name
        self.created_at = created_at
        self.country_code = country_code

class UsersSchema(ma.Schema):
    class Meta:
        # Fields to expose
        fields = ('id' ,'full_name', 'created_at','country_code')
        #fields = ('id', 'full_name')


user_schema = UsersSchema()
users_schema = UsersSchema(many=True)



@app.route("/query1", methods=["GET"])
def get_user_by_one():
    name= request.json["full_name"]
    user = Users.query.filter_by(full_name = name).first()
    result = user_schema.dump(user)

    return jsonify(result)

@app.route("/query", methods=["GET"])
def get_user_by():
    name=  request.json.get('full_name', "Jacek")
    #cc=    request.json['country_code']
    user = Users.query.filter_by(full_name = name, country_code = 2).first()
    result = user_schema.dump(user)
    return jsonify(result)


@app.route("/query/part", methods=["POST"])
def get_user_by_part():
    name= request.json["full_name"]
    ccode= request.json["country_code"]
    user = Users.query.filter_by(full_name= name, country_code = ccode).first()
    result = user_schema.dump(user)

    return jsonify(result)


@app.route("/sortowanie", methods=["GET"])
def get_users():
    all_users = Users.query.order_by(Users.full_name).all()
    result = users_schema.dump(all_users)
    return jsonify(result)


@app.route('/user/<username>')
def show_user(username):
    user = Users.query.filter_by(full_name=username).first_or_404(description='There is no data with {}'.format(username))
    return render_template('show_user.html', user=user)

@app.route("/<id>", methods=["GET"])
def get_user(id):
    user = Users.query.get(id)
    result = user_schema.dump(user)
    return result


@app.route("/dodajBIM", methods=["POST"])
def add_user():
    name= request.json["full_name"]
    code= request.json["country_code"]
    new_user = Users(name, datetime.now(),code)
    db.session.add(new_user)
    db.session.commit()  # PK increment
    user = Users.query.get(new_user.id)
    return user_schema.jsonify(user)


@app.route('/z')
def hello_world():
    return 'Pozdrawiamy!!!'

