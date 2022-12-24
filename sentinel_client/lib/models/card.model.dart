import 'package:sentinel_client/models/cardType.model.dart';
// import 'package:sentinel_client/models/user.model.dart';

class UserCard {
  final String id;
  /*final*/ String value;
  /*final*/ UserCardType type;
  /*final*/ String? description;
  /*final*/ bool hasValidity;
  /*final*/ DateTime? validSince;
  /*final*/ DateTime? validUntil;
  /*final*/ bool isEnabled;
  // final User lastChangedBy; // ? - not full User - _id, user_number, name, surname, username

  UserCard(
    this.id,
    this.value,
    this.type,
    this.description,
    this.hasValidity,
    this.validSince,
    this.validUntil,
    this.isEnabled,
  );

  UserCard.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        value = json['value'],
        type = UserCardType.fromJson(json['type']),
        description = json['description'],
        hasValidity = json['has_validity'],
        validSince = json['valid_since'],
        validUntil = json['valid_until'],
        isEnabled = json['is_enabled'];
}
