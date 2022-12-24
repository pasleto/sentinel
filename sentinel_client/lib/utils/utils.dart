class AppUtils {
  static String stripAccents(String text) {
    var inChrs = 'ÀÁÂÃÄÅàáâãäåÒÓÔÕÕÖØòóôõöøÈÉÊËĚèéêëěðČÇçčÐĎďÌÍÎÏìíîïĽľÙÚÛÜŮùúûüůŇÑñňŘřŠšŤťŸÝÿýŽž';
    var outChrs = 'AAAAAAaaaaaaOOOOOOOooooooEEEEEeeeeeeCCccDDdIIIIiiiiLlUUUUUuuuuuNNnnRrSsTtYYyyZz';
    for (int i = 0; i < inChrs.length; i++) {
      text = text.replaceAll(inChrs[i], outChrs[i]);
    }
    return text;
  }
}
