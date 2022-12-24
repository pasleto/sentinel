import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';

// import '../../utils/deferred_widget.dart';

class AccessControlManagementDashboardPage extends StatefulWidget {
  const AccessControlManagementDashboardPage({super.key});

  @override
  State<AccessControlManagementDashboardPage> createState() => _AccessControlManagementDashboardPageState();
}

class _AccessControlManagementDashboardPageState extends State<AccessControlManagementDashboardPage> with PageMixin {
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
      child: Text('Access Control Management - Dashboard Page'),
    );
    // return const DeferredLoadingPlaceholder();
  }
}
