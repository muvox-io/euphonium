import { Component, AnyComponent, JSX, FunctionComponent } from "preact";

export default function ErrorBoundaryWrapper<P>(
  Component: FunctionComponent<P>
) {
  return (props: any) => {
    return (
      <ErrorBoundary component={Component} componentProps={props}>
        <Component {...props} />
      </ErrorBoundary>
    );
  };
}

export interface ErrorBoundaryProps {
  children: JSX.Element;
  component: any;
  componentProps: any;
}

export interface ErrorBoundaryState {
  error: Error | null;
  timestamp: number | null;
  lastComponentProps: any;
}

export class ErrorBoundary extends Component<
  ErrorBoundaryProps,
  ErrorBoundaryState
> {
  constructor(props: ErrorBoundaryProps) {
    super(props);
    this.state = { error: null, timestamp: null, lastComponentProps: null };
  }

  static getDerivedStateFromError(error: Error) {
    // Update state so the next render will show the fallback UI.
    return { error, timestamp: Date.now() };
  }

  static getDerivedStateFromProps(
    props: ErrorBoundaryProps,
    state: ErrorBoundaryState
  ) {
    // do a shallow compare of componment props and clear error if they are diffrent
    // also check the timestamp, and don't clear if the rror happened in the last 100ms
    if (
      state.lastComponentProps &&
      state.error &&
      JSON.stringify(props.componentProps) !==
        JSON.stringify(state.lastComponentProps) &&
      Date.now() - state.timestamp! < 100
    ) {
      return {
        error: null,
        timestamp: null,
        lastComponentProps: props.componentProps,
      };
    }

    return {
      lastComponentProps: props.componentProps,
    };
  }

  componentDidCatch(error: Error, errorInfo: any) {
    // You can also log the error to an error reporting service
    console.error(error, errorInfo);
  }

  render() {
    if (this.state.error) {
      // You can render any custom fallback UI
      return (
        <div class="bg-app-primary md:min-w-[600px] rounded-2xl p-7 m-7 text-danger">
          <div class="text-2xl mb-3 text-rose-600">An error has occured!</div>
          <div class="text-m mt-2 text-app-text-secondary">
            <code>Error: {this.state.error.message}</code>
            <div>
              <code>Stack trace:</code>
            </div>
            <pre>
              <code>{this.state.error.stack}</code>
            </pre>
            <br />
            <br />
            <code>
              <pre>
                Component: {this.props?.component?.name}
                <br />
                Timestamp: {new Date(this.state.timestamp!).toString()}
              </pre>
            </code>
          </div>
        </div>
      );
    }

    return this.props.children;
  }
}
