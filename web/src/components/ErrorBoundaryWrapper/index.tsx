import { Component, AnyComponent ,JSX } from "preact";

export default function ErrorBoundaryWrapper<P, S>(Component: AnyComponent<P, S>) {
  return (props: P) => {
    return (
      <ErrorBoundary>
        <Component {...props} />
      </ErrorBoundary>
    );
  };
}

export interface ErrorBoundaryProps {
  children: JSX.Element;
}

export interface ErrorBoundaryState {
  error: Error | null;
}

export class ErrorBoundary extends Component<
  ErrorBoundaryProps,
  ErrorBoundaryState
> {
  constructor(props: ErrorBoundaryProps) {
    super(props);
    this.state = { error: null };
  }

  static getDerivedStateFromError(error: Error) {
    // Update state so the next render will show the fallback UI.
    return { error };
  }

  componentDidCatch(error: Error, errorInfo: any) {
    // You can also log the error to an error reporting service
    console.error(error, errorInfo);
  }

  render() {
    if (this.state.error) {
      // You can render any custom fallback UI
      return (
        <div class="bg-app-primary md:min-w-[600px] rounded-2xl p-7 m-7 text-red-100">
          <div class="text-2xl mb-3 text-rose-600">An error has occured!</div>
          <div class="text-m mt-2 text-app-text-secondary">
            <code>Error: {this.state.error.message}</code>
            <div><code>Stack trace:</code></div>
            <pre><code>{this.state.error.stack}</code></pre>
          </div>
        </div>
      );
    }

    return this.props.children;
  }
}
