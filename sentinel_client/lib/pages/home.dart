import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> with PageMixin {
  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return const Center(
      child: Text('Home Page'),
    );
    // return ScaffoldPage.scrollable(
    //   children: [
    //     Text('Home'),
    //     Text('Page'),
    //   ],
    // );
  }
}
