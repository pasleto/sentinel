import 'dart:convert';
import 'dart:typed_data';

import 'package:sentinel_client/providers/disposable_provider.dart';
import 'package:sentinel_client/models/user.model.dart';

class AppUser extends DisposableProvider {
  User _user = User(
    '',
    '',
    '',
    '',
    '',
    '',
    '',
    '',
    '',
    false,
    false,
    false,
    '',
    '',
    false,
    null,
    // base64Decode(
    //     'iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAnFBMVEXn5+c9PT3q6ura2to/Pz/g4ODV1dVUVFRDQ0Pi4uJKSkrm5ubHx8fAwMCjo6Pk5OSxsbFycnJfX19PT09NTU1GRkbQ0NCCgoJ+fn6oqKiXl5eRkZFjY2NRUVFBQUE6OjrX19fLy8vCwsK9vb24uLienp6bm5t6enp5eXlra2tmZmZcXFzc3Nytra2goKCLi4uEhIRpaWmGhoZvb2+MNFbWAAACz0lEQVRo3u1Y2XLiMBC0JB/I9wG+zX0FAiHJ///bzpgtagEHV1l62ZT6gQfJNS31aDQtNAUFBQUFBQWFXwdKOTMBjFOqyQcNFv6utprGqnf+IpBOESwvjU7+Qm8uy0Bu/HmVkDsk1VymPIvv6+rtJo4b+7qL74U8mRYHAhgdylXKWLoqDyMCOCxkxTffCCAumIZrhh9WxDjyZsqJb2SoyD79dyzdo2aZISUB4QTi54zeDbIcGCahjDQwFOjTpA+05ieKxGRsADI6XtOn8fUY8i5hC0GGYvPnCd5OBOIldgStcQPPW4DcHOfCCnlQV/G8kzqGyvOoKMEJUrA3Oo/vHpJwEibwdTL7cLqmnI8Z0X0pBBXtnKqkEVyCzvN1kUKAZRB15yCSUQjUhXqy0q6p1IIKdIUrjdWETIuuOiimhNTid4Xzcb1zuu8oOF+ioCsbpPafJ3xIjr2iEtpBTiALHn2scAuGcxkNgS4b7I8uvc89dtFmKaUtO1tswceQ01t4Hh6xSW8dST3zS0dHkXkGbWF4mY1d9MvQJIFlyEDGUXlar09lNCYYP2OaNLyXGBNVmUxQL2Qr32U6rzQiD4hSKi88O8fTR4JpfGaSKBwvuobXbauOotqy9StF5Mnx1YVFEMnO90xmGMz0/F1CEFYhwWPz7aSNVbmc0ttLxK1a1smWC6+/jT/KXe2+kjU3H7UMgaD+BcYfb4xn42VsxshQCFXz9Uqzwu6mH7aTQsaFRbj+8KfpEPcQiRT0eQr6b5wfBdxAHqZngZdH3N75Pb0iNgcTbHTQ2KWvLAGkQd8MvuLwCVBpL1Hh4+FdoBsnLn3tapLhnZmWsLod7yn0HXxUDiPge4LWsN9akj0fVgQ3/9/7fhhWCinIa5m9R9mCRKXD7IpNZjXrt5YzYg+yL9Q9Jklu9PuyJDkOs8Ac/3xyeu9bBp9xTUFBQUFBQUHhf8cfdJYm2hAn4e0AAAAASUVORK5CYII='),
    'iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAnFBMVEXn5+c9PT3q6ura2to/Pz/g4ODV1dVUVFRDQ0Pi4uJKSkrm5ubHx8fAwMCjo6Pk5OSxsbFycnJfX19PT09NTU1GRkbQ0NCCgoJ+fn6oqKiXl5eRkZFjY2NRUVFBQUE6OjrX19fLy8vCwsK9vb24uLienp6bm5t6enp5eXlra2tmZmZcXFzc3Nytra2goKCLi4uEhIRpaWmGhoZvb2+MNFbWAAACz0lEQVRo3u1Y2XLiMBC0JB/I9wG+zX0FAiHJ///bzpgtagEHV1l62ZT6gQfJNS31aDQtNAUFBQUFBQWFXwdKOTMBjFOqyQcNFv6utprGqnf+IpBOESwvjU7+Qm8uy0Bu/HmVkDsk1VymPIvv6+rtJo4b+7qL74U8mRYHAhgdylXKWLoqDyMCOCxkxTffCCAumIZrhh9WxDjyZsqJb2SoyD79dyzdo2aZISUB4QTi54zeDbIcGCahjDQwFOjTpA+05ieKxGRsADI6XtOn8fUY8i5hC0GGYvPnCd5OBOIldgStcQPPW4DcHOfCCnlQV/G8kzqGyvOoKMEJUrA3Oo/vHpJwEibwdTL7cLqmnI8Z0X0pBBXtnKqkEVyCzvN1kUKAZRB15yCSUQjUhXqy0q6p1IIKdIUrjdWETIuuOiimhNTid4Xzcb1zuu8oOF+ioCsbpPafJ3xIjr2iEtpBTiALHn2scAuGcxkNgS4b7I8uvc89dtFmKaUtO1tswceQ01t4Hh6xSW8dST3zS0dHkXkGbWF4mY1d9MvQJIFlyEDGUXlar09lNCYYP2OaNLyXGBNVmUxQL2Qr32U6rzQiD4hSKi88O8fTR4JpfGaSKBwvuobXbauOotqy9StF5Mnx1YVFEMnO90xmGMz0/F1CEFYhwWPz7aSNVbmc0ttLxK1a1smWC6+/jT/KXe2+kjU3H7UMgaD+BcYfb4xn42VsxshQCFXz9Uqzwu6mH7aTQsaFRbj+8KfpEPcQiRT0eQr6b5wfBdxAHqZngZdH3N75Pb0iNgcTbHTQ2KWvLAGkQd8MvuLwCVBpL1Hh4+FdoBsnLn3tapLhnZmWsLod7yn0HXxUDiPge4LWsN9akj0fVgQ3/9/7fhhWCinIa5m9R9mCRKXD7IpNZjXrt5YzYg+yL9Q9Jklu9PuyJDkOs8Ac/3xyeu9bBp9xTUFBQUFBQUHhf8cfdJYm2hAn4e0AAAAASUVORK5CYII=',
    // null,
  );
  User get user => _user;
  set user(User user) {
    _user = user;
    notifyListeners();
  }

  bool _isLogged = false;
  bool get isLogged => _isLogged;
  set isLogged(bool isLogged) {
    _isLogged = isLogged;
    notifyListeners();
  }

  bool _isFetched = false;
  bool get isFetched => _isFetched;
  set isFetched(bool isFetched) {
    _isFetched = isFetched;
    notifyListeners();
  }

  bool _isRefreshing = false;
  bool get isRefreshing => _isRefreshing;
  set isRefreshing(bool isRefreshing) {
    _isRefreshing = isRefreshing;
    notifyListeners();
  }

  // Uint8List getImageData() {
  //   return base64Decode(_user.image);
  // }

  void setRefreshing(bool state) {
    _isRefreshing = state;
    notifyListeners();
  }

  void setUserDataFromJson(var json) {
    _user = User.fromJson(json);
    // print(json);
    print(_user);

    // Future.delayed(const Duration(seconds: 5), () {
    //   _isLogged = true;
    //   notifyListeners();
    // });

    _isLogged = true;
    _isFetched = true;
    _isRefreshing = false;
    notifyListeners();
  }

  @override
  void disposeValues() {
    // ? implement disposeValues
    _isLogged = false;
    _isFetched = false;
    _user = User(
      '',
      '',
      '',
      '',
      '',
      '',
      '',
      '',
      '',
      false,
      false,
      false,
      '',
      '',
      false,
      null,
      // base64Decode(
      //     'iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAnFBMVEXn5+c9PT3q6ura2to/Pz/g4ODV1dVUVFRDQ0Pi4uJKSkrm5ubHx8fAwMCjo6Pk5OSxsbFycnJfX19PT09NTU1GRkbQ0NCCgoJ+fn6oqKiXl5eRkZFjY2NRUVFBQUE6OjrX19fLy8vCwsK9vb24uLienp6bm5t6enp5eXlra2tmZmZcXFzc3Nytra2goKCLi4uEhIRpaWmGhoZvb2+MNFbWAAACz0lEQVRo3u1Y2XLiMBC0JB/I9wG+zX0FAiHJ///bzpgtagEHV1l62ZT6gQfJNS31aDQtNAUFBQUFBQWFXwdKOTMBjFOqyQcNFv6utprGqnf+IpBOESwvjU7+Qm8uy0Bu/HmVkDsk1VymPIvv6+rtJo4b+7qL74U8mRYHAhgdylXKWLoqDyMCOCxkxTffCCAumIZrhh9WxDjyZsqJb2SoyD79dyzdo2aZISUB4QTi54zeDbIcGCahjDQwFOjTpA+05ieKxGRsADI6XtOn8fUY8i5hC0GGYvPnCd5OBOIldgStcQPPW4DcHOfCCnlQV/G8kzqGyvOoKMEJUrA3Oo/vHpJwEibwdTL7cLqmnI8Z0X0pBBXtnKqkEVyCzvN1kUKAZRB15yCSUQjUhXqy0q6p1IIKdIUrjdWETIuuOiimhNTid4Xzcb1zuu8oOF+ioCsbpPafJ3xIjr2iEtpBTiALHn2scAuGcxkNgS4b7I8uvc89dtFmKaUtO1tswceQ01t4Hh6xSW8dST3zS0dHkXkGbWF4mY1d9MvQJIFlyEDGUXlar09lNCYYP2OaNLyXGBNVmUxQL2Qr32U6rzQiD4hSKi88O8fTR4JpfGaSKBwvuobXbauOotqy9StF5Mnx1YVFEMnO90xmGMz0/F1CEFYhwWPz7aSNVbmc0ttLxK1a1smWC6+/jT/KXe2+kjU3H7UMgaD+BcYfb4xn42VsxshQCFXz9Uqzwu6mH7aTQsaFRbj+8KfpEPcQiRT0eQr6b5wfBdxAHqZngZdH3N75Pb0iNgcTbHTQ2KWvLAGkQd8MvuLwCVBpL1Hh4+FdoBsnLn3tapLhnZmWsLod7yn0HXxUDiPge4LWsN9akj0fVgQ3/9/7fhhWCinIa5m9R9mCRKXD7IpNZjXrt5YzYg+yL9Q9Jklu9PuyJDkOs8Ac/3xyeu9bBp9xTUFBQUFBQUHhf8cfdJYm2hAn4e0AAAAASUVORK5CYII='),
      'iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAnFBMVEXn5+c9PT3q6ura2to/Pz/g4ODV1dVUVFRDQ0Pi4uJKSkrm5ubHx8fAwMCjo6Pk5OSxsbFycnJfX19PT09NTU1GRkbQ0NCCgoJ+fn6oqKiXl5eRkZFjY2NRUVFBQUE6OjrX19fLy8vCwsK9vb24uLienp6bm5t6enp5eXlra2tmZmZcXFzc3Nytra2goKCLi4uEhIRpaWmGhoZvb2+MNFbWAAACz0lEQVRo3u1Y2XLiMBC0JB/I9wG+zX0FAiHJ///bzpgtagEHV1l62ZT6gQfJNS31aDQtNAUFBQUFBQWFXwdKOTMBjFOqyQcNFv6utprGqnf+IpBOESwvjU7+Qm8uy0Bu/HmVkDsk1VymPIvv6+rtJo4b+7qL74U8mRYHAhgdylXKWLoqDyMCOCxkxTffCCAumIZrhh9WxDjyZsqJb2SoyD79dyzdo2aZISUB4QTi54zeDbIcGCahjDQwFOjTpA+05ieKxGRsADI6XtOn8fUY8i5hC0GGYvPnCd5OBOIldgStcQPPW4DcHOfCCnlQV/G8kzqGyvOoKMEJUrA3Oo/vHpJwEibwdTL7cLqmnI8Z0X0pBBXtnKqkEVyCzvN1kUKAZRB15yCSUQjUhXqy0q6p1IIKdIUrjdWETIuuOiimhNTid4Xzcb1zuu8oOF+ioCsbpPafJ3xIjr2iEtpBTiALHn2scAuGcxkNgS4b7I8uvc89dtFmKaUtO1tswceQ01t4Hh6xSW8dST3zS0dHkXkGbWF4mY1d9MvQJIFlyEDGUXlar09lNCYYP2OaNLyXGBNVmUxQL2Qr32U6rzQiD4hSKi88O8fTR4JpfGaSKBwvuobXbauOotqy9StF5Mnx1YVFEMnO90xmGMz0/F1CEFYhwWPz7aSNVbmc0ttLxK1a1smWC6+/jT/KXe2+kjU3H7UMgaD+BcYfb4xn42VsxshQCFXz9Uqzwu6mH7aTQsaFRbj+8KfpEPcQiRT0eQr6b5wfBdxAHqZngZdH3N75Pb0iNgcTbHTQ2KWvLAGkQd8MvuLwCVBpL1Hh4+FdoBsnLn3tapLhnZmWsLod7yn0HXxUDiPge4LWsN9akj0fVgQ3/9/7fhhWCinIa5m9R9mCRKXD7IpNZjXrt5YzYg+yL9Q9Jklu9PuyJDkOs8Ac/3xyeu9bBp9xTUFBQUFBQUHhf8cfdJYm2hAn4e0AAAAASUVORK5CYII=',
      // null,
    );
    notifyListeners();
  }
}
