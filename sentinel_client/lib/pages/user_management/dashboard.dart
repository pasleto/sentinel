import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';

class UserManagementDashboardPage extends StatefulWidget {
  const UserManagementDashboardPage({super.key});

  @override
  State<UserManagementDashboardPage> createState() => _UserManagementDashboardPageState();
}

class _UserManagementDashboardPageState extends State<UserManagementDashboardPage> with PageMixin {
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
      child: Text('User Management - Dashboard Page'),
    );
  }
}
