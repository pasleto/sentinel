import 'package:fluent_ui/fluent_ui.dart';
import 'package:sentinel_client/widgets/page.dart';

class CarFleetManagementDashboardPage extends StatefulWidget {
  const CarFleetManagementDashboardPage({super.key});

  @override
  State<CarFleetManagementDashboardPage> createState() => _CarFleetManagementDashboardPageState();
}

class _CarFleetManagementDashboardPageState extends State<CarFleetManagementDashboardPage> with PageMixin {
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
      child: Text('Car Fleet Management - Dashboard Page'),
    );
  }
}
