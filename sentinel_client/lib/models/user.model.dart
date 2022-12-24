import 'dart:convert';
import 'dart:typed_data';

import 'package:sentinel_client/models/card.model.dart';

class User {
  final String id;
  /*final*/ String userNumber;
  /*final*/ String name;
  /*final*/ String surname;
  /*final*/ String username;
  /*final*/ String? email;
  /*final*/ String? mobile;
  /*final*/ String? telephone;
  /*final*/ String departmentId;
  /*final*/ bool isAdmin;
  /*final*/ bool loginEnabled;
  /*final*/ bool isLdapUser;
  /*final*/ String? ldapSid;
  /*final*/ String? jobTitle;
  /*final*/ bool isProtected;
  /*final*/ List<UserCard>? cards;
  /*final*/ String image;

  User(
    this.id,
    this.userNumber,
    this.name,
    this.surname,
    this.username,
    this.email,
    this.mobile,
    this.telephone,
    this.departmentId,
    this.isAdmin,
    this.loginEnabled,
    this.isLdapUser,
    this.ldapSid,
    this.jobTitle,
    this.isProtected,
    this.cards,
    this.image,
  );

  User.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        userNumber = json['user_number'],
        name = json['name'],
        surname = json['surname'],
        username = json['username'],
        email = json['email'], // ?
        mobile = json['mobile'], // ?
        telephone = json['telephone'], // ?
        departmentId = json['department'],
        isAdmin = json['is_admin'],
        loginEnabled = json['login_enabled'],
        isLdapUser = json['is_ldap_user'],
        ldapSid = json['ldap_sid'], // ?
        jobTitle = json['job_title'], // ?
        isProtected = json['is_protected'],
        cards = json['cards'] != null ? List<UserCard>.from(json['cards'].map((model) => UserCard.fromJson(model))) : null, // ?
        image = json['image'];

  Uint8List getImageData() {
    return base64Decode(image);
  }
}
