@app.route("/strona/dodaj", methods=["GET", "POST"])
def get_users_nasz_dodaj():
    form = DodajUzytkownka()
    if request.method == 'POST':
            nazwisko= form.nazwisko.data
            kod= int(form.kraj.data)
            nowa_data = form.data_nasza.data
            new_user = Users(nazwisko, datetime.now(),kod)
            db.session.add(new_user)
            db.session.commit()

            user = Users.query.all()
            result = users_schema.dump(user)
            flash('Dodano uzytkownika!!!')
            return render_template("stronawww.html",title='Tytuł testy nauki robienia stron',  users= result)
   return render_template("strona_nasza_dodaj.html", form= form)
   
   
@app.route("/strona/nasza", methods=["GET"])
def get_users_nasz():
    #all_users = Users.query.all()
    all_users = Users.query.order_by(Users.full_name).all()
    result = users_schema.dump(all_users)
    #return jsonify(result)
    return render_template('stronawww.html', title='Tytuł testy nauki robienia stron', users=result)
