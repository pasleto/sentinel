class UserCardType {
  final String id;
  final String name;
  final String? description;
  final bool isSecure;
  final String frequency; // enum -> 'LF (125 KHz)', 'HF (13.56 MHz)'
  final bool isProtected;
  // final List<Card> cards;

  UserCardType(
    this.id,
    this.name,
    this.description,
    this.isSecure,
    this.frequency,
    this.isProtected,
  );

  UserCardType.fromJson(Map<String, dynamic> json)
      : id = json['_id'],
        name = json['name'],
        description = json['description'],
        isSecure = json['is_secure'],
        frequency = json['frequency'],
        isProtected = json['is_protected'];
}
