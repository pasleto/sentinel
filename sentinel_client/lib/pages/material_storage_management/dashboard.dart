import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';

class MaterialStorageManagementDashboardPage extends StatefulWidget {
  const MaterialStorageManagementDashboardPage({super.key});

  @override
  State<MaterialStorageManagementDashboardPage> createState() => _MaterialStorageManagementDashboardPageState();
}

class _MaterialStorageManagementDashboardPageState extends State<MaterialStorageManagementDashboardPage> with PageMixin {
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
      child: Text('Material Storage Management - Dashboard Page'),
    );
  }
}
